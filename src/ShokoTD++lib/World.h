#pragma once
#include "vmath.h"
#include "GridSquare.h"
#include <vector>

using std::vector;
using std::string;

namespace
{
	struct TopLeft
	{
		bool top;
		bool left;
	};
}

namespace SquareType
{
	enum Enum
	{
		Empty, Rocket, Hole, Cross, NorthArrow, SouthArrow, EastArrow, WestArrow, NorthSpawner, SouthSpawner, EastSpawner, WestSpawner,
	};

	Direction::Enum GetDirection(Enum _square_type);
	Enum ArrowFromDirection(Direction::Enum _direction);
	Enum SpawnerFromDirection(Direction::Enum _direction);
}

namespace WorldState
{
	enum Enum
	{
		OK, FileLoadError, Victory, Defeat
	};
}

struct ArrowRecord
{
	Vector2i Position;
	Direction::Enum Direction;
};

struct Spawner
{
	Vector2i Position;
	Direction::Enum Direction;
};

class Walker;

struct WaveComponent
{
	std::string enemy_type;
	float weight;
};

struct Wave
{
	vector<WaveComponent> enemy_types;
	int enemy_count;
	float spawn_time_gap;
	float start_time;
	static bool IsDone(Wave _wave)
	{
		return _wave.enemy_count <= 0;
	}

	Walker* Spawn();
};

class World
{
protected:
	Vector2i size_;
	vector<vector<TopLeft>> walls_;
	vector<vector<SquareType::Enum>> special_squares_;
	vector<vector<int>> special_squares_count_;
	vector<Walker*> enemies_;
	vector<Walker*> just_dead_enemies_;
	vector<Walker*> dead_enemies_;
	vector<Vector2f> problem_points_;
	vector<Spawner> spawners_;
	
	float sum_time_;
	float spawn_pause_time_;
	vector<Wave> active_waves_;
	vector<Wave> finsished_waves_;

	string name_;
	string filename_;
	WorldState::Enum state_;
	int neutral_kills_;
	int evil_kills_;
	int good_kills_;
	int max_lives_;
	int lives_;


public:
	World(void);
	World(std::string _filename);
	~World(void);

	//Advances all the mice and cats and collides everything
	WorldState::Enum Tick(float _dt);
	//Resets the level to it's starting state(Mice reset, placed arrows restored)
	void Reset();
	//Saves the level to XML
	void SaveAs(string _filename);

	//Gets / sets the size of the level
	Vector2i GetSize(){return size_;}
	void SetSize(Vector2i _size);
	//Gets / sets the walls around a coordinate
	GridSquare GetGridSquare(Vector2i _point);
	void SetGridSquare(Vector2i _point, GridSquare _gs);
	void ToggleWall(Vector2i _position, Direction::Enum _direction);


	//Enemy query functions
	std::vector<Walker*> GetEnemies(){return enemies_;}
	std::vector<Walker*> GetDeadEnemies(){return dead_enemies_;}
	//Wave query functions
	std::vector<Wave> GetActiveWaves(){return active_waves_;}
	std::vector<Wave> GetFinishedWaves(){return finsished_waves_;}

	//Gets / sets the square type
	SquareType::Enum GetSquareType(Vector2i _point);
	int GetSquareCount(Vector2i _point);
	bool SetSquareType(Vector2i _point, SquareType::Enum _square_type);
	void ToggleRocket(Vector2i _position);
	void ToggleHole(Vector2i _position);
	//TODO void ToggleSpawner(Vector2i _position, Direction::Enum

	//Spawning functions
	vector<Spawner> GetSpawners(){return spawners_;}
	void SpawnPause(float _duration){spawn_pause_time_ = _duration;}


	//Gets / sets the name of the level
	string GetName(){return name_;}
	void SetName(string _name){name_ = _name;}

	string GetFilename(){return filename_;}

	//Arrow helper functions and stock querier
	void ClearArrow(Vector2i _position);
	void ToggleArrow(Vector2i _position, Direction::Enum _direction, int _count);
	void ClearArrows();
	int GetArrowsInUse();

	// Problem point functions
	vector<Vector2f> GetProblemPoints(){return problem_points_;}

	//Lives and score functions
	int GetLives(){return lives_;}
	int GetMaxLives(){return max_lives_;}
	int GetNeutralKills(){return neutral_kills_;}
	int GetGoodKills(){return good_kills_;}
	int GetEvilKills(){return evil_kills_;}

	//Gets the error state
	bool GetError(){return state_ == WorldState::FileLoadError;}

	void WalkerReachNewSquare(Walker* _walker);
};
