#pragma once
#include "vmath.h"
#include "Direction.h"

class World;

namespace WalkerState
{
	enum Enum
	{
		Live, Killed
	};
}

namespace EnemyTypes
{
	class EnemyType;
}

class Walker
{
protected:
	Vector2f position_;
	Vector2f initial_position_;
	Direction::Enum direction_;
	Direction::Enum initial_direction_;
	float speed_;
	float health_;
	float max_health_;
	float evil_damage_;
	float good_damage_;
	EnemyTypes::EnemyType* enemy_type_;
	World* pWorld_;

	WalkerState::Enum walker_state_;
	float death_time_;
	float slow_time_;
	float slow_factor_;

	float craze_time_;

public:
	Walker(void);
	~Walker(void);
	static int Total_created;
	static int Total_destroyed;

	/* Gets and sets the position */
	Vector2f GetPosition(){return position_;}
	void SetPosition(Vector2f _position){initial_position_ = _position; position_ = _position;}
	/* Gets and sets the direction */
	Direction::Enum GetDirection(){return direction_;}
	void SetDirection(Direction::Enum _direction){initial_direction_ = _direction; direction_ = _direction;}
	/* Reacts to an arrow on the world */
	void EncounterArrow(Direction::Enum _direction){direction_ = _direction;}
	/* Gets and sets the walker speed */
	void SetSpeed(float _speed);
	float GetSpeed(){return speed_;}
	void SlowDown(float _slowtime, float _slowfactor){slow_time_ = _slowtime; slow_factor_ = _slowfactor;}
	void SetCrazed(float _craze_time){craze_time_ = _craze_time;}
	/* Gets and sets the walker health */
	void SetHealth(float _health){health_ = _health; max_health_ = _health;}
	float GetMaxHealth(){return max_health_;}
	float GetHealth(){return health_;}
	void TakeGoodDamage(float _damage){health_ -= _damage; good_damage_ += _damage;}
	void TakeEvilDamage(float _damage){health_ -= _damage; evil_damage_ += _damage;}
	float MoreEvilThanGood(){return evil_damage_ > good_damage_;}
	/* Gets and sets the walker type */
	void SetEnemyType(EnemyTypes::EnemyType* _enemy_type);
	EnemyTypes::EnemyType* GetEnemyType(){return enemy_type_;}


	/* Gets and sets the world */
	World* GetWorld(){return pWorld_;}
	void SetWorld(World* _pWorld){pWorld_ = _pWorld;}


	
	/* Causes the walker to advance for a number of seconds */
	void Advance(float _timespan);
	/* Causes the walker to examine environment, and if finding it's route blocked it will turn */
	bool DoTurns();
	/* Resets the walker to the initial position */
	void Reset();
	/* Counts the time spent dead for the walker */
	void DeathTick(float _timespan){death_time_ += _timespan;};
	float GetDeathTime(){return death_time_;}
	
	/* Changes the walker state to dead */
	void Kill();
	WalkerState::Enum GetWalkerState();
};
