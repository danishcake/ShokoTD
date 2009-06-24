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
		Empty, Rocket, Hole, NorthArrow, SouthArrow, EastArrow, WestArrow, HalfNorthArrow, HalfSouthArrow, HalfEastArrow, HalfWestArrow, DestroyedNorthArrow, DestroyedSouthArrow, DestroyedEastArrow, DestroyedWestArrow
	};

	Direction::Enum GetDirection(Enum _square_type);
	Enum FromDirection(Direction::Enum _direction);
	Enum Diminish(Enum _square_type);
	Enum RestoreToFull(Enum _square_type);
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

class Walker;

class World
{
protected:
	Vector2i size_;
	vector<vector<TopLeft>> walls_;
	vector<vector<SquareType::Enum>> special_squares_;
	vector<Walker*> enemies_;
	vector<Walker*> just_dead_enemies_;
	vector<Walker*> dead_enemies_;
	vector<Vector2f> problem_points_;


	vector<Direction::Enum> arrows_;
	vector<ArrowRecord> solution_arrows_;
	bool unlimited_arrow_stock_;

	int remaining_enemies_;
	string name_;
	string filename_;
	WorldState::Enum state_;

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

	//Gets / sets the square type
	SquareType::Enum GetSquareType(Vector2i _point);
	bool SetSquareType(Vector2i _point, SquareType::Enum _square_type);
	void ToggleRocket(Vector2i _position);
	void ToggleHole(Vector2i _position);

	//Gets / sets the name of the level
	string GetName(){return name_;}
	void SetName(string _name){name_ = _name;}

	string GetFilename(){return filename_;}

	//Arrow helper functions and stock querier
	void AddArrow(Direction::Enum _arrow){arrows_.push_back(_arrow);}
	vector<Direction::Enum> GetArrows(){return arrows_;}
	void ClearArrow(Vector2i _position);
	void ToggleArrow(Vector2i _position, Direction::Enum _direction);
	void ToggleNorthArrow(Vector2i _position){ToggleArrow(_position, Direction::North);}
	void ToggleSouthArrow(Vector2i _position){ToggleArrow(_position, Direction::South);}
	void ToggleEastArrow(Vector2i _position){ToggleArrow(_position, Direction::East);}
	void ToggleWestArrow(Vector2i _position){ToggleArrow(_position, Direction::West);}
	void ClearArrows();
	void SetArrowStockUnlimited(){unlimited_arrow_stock_ = true;}


	void LoadSolution();
	vector<Vector2f> GetProblemPoints(){return problem_points_;}
	

	//Gets the error state
	bool GetError(){return state_ == WorldState::FileLoadError;}

	void WalkerReachNewSquare(Walker* _walker);
};
