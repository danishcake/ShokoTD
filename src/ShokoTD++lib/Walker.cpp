#include "Walker.h"
#include "EnemyTypes.h"
#include <cmath>
#include "World.h"

int Walker::Total_created = 0;
int Walker::Total_destroyed = 0;


Walker::Walker(void)
{
	initial_direction_ = Direction::North;
	direction_ = initial_direction_;
	
	initial_position_ = Vector2f(0, 0);
	position_ = initial_position_;

	speed_ = 1.0f;

	pWorld_ = NULL;
	Total_created++;
	death_time_ = 0;
	walker_state_ = WalkerState::Live;
	enemy_type_ = NULL;
	health_ = 100;
	evil_damage_ = 0;
	good_damage_ = 0;
	slow_factor_ = 1;
	slow_time_ = 0;
	craze_time_ = 0;
}

Walker::~Walker(void)
{
	Total_destroyed++;
}

bool Walker::DoTurns()
{
	if(pWorld_)
	{
		if(position_.x >= pWorld_->GetSize().x)
			position_.x -= pWorld_->GetSize().x;
		if(position_.y >= pWorld_->GetSize().y)
			position_.y -= pWorld_->GetSize().y;
		if(position_.y <= -1.0f)
			position_.y += pWorld_->GetSize().y;
		if(position_.x <= -1.0f)
			position_.x += pWorld_->GetSize().x;

		if(craze_time_ > 0)
		{
			direction_ = (Direction::Enum)(rand() %4);
		}
		Direction::Enum old_direction = direction_;
		pWorld_->WalkerReachNewSquare(this);
		GridSquare gs = pWorld_->GetGridSquare(Vector2i(position_.x, position_.y));
		direction_ = gs.GetTurnDirection(direction_);
		return old_direction != direction_;
	}
	return false;
}

void Walker::Advance(float _timespan)
{
	if(slow_time_ > 0)
		slow_time_ -= _timespan;
	else
		slow_factor_ = 1;
	if(craze_time_ > 0)
		craze_time_ -= _timespan;
	float distance_to_go = speed_ * _timespan * slow_factor_;
	while(distance_to_go > 0)
	{	
		float distance_to_next = 0;
		switch(direction_)
		{
			case Direction::Stopped:
				return;
			break;
			case Direction::North:
				distance_to_next = position_.y - floor(position_.y);
				if(distance_to_next == 0)
				{
					distance_to_next = 1.0f;
					if(DoTurns())
						continue;
				}

				if(distance_to_go >= distance_to_next)
				{
					distance_to_go -= distance_to_next;
					position_ += Vector2f( 0.0f, -1.0f) * distance_to_next;
					DoTurns();
				} else
				{
					position_ += Vector2f( 0.0f, -1.0f) * distance_to_go;
					distance_to_go = 0;
				}

				break;
			case Direction::South:
				distance_to_next = ceil(position_.y) - position_.y;
				if(distance_to_next == 0)
				{
					distance_to_next = 1.0f;
					if(DoTurns())
						continue;
				}

				if(distance_to_go >= distance_to_next)
				{
					distance_to_go -= distance_to_next;
					position_ += Vector2f( 0.0f,  1.0f) * distance_to_next;
					DoTurns();
				} else
				{
					position_ += Vector2f( 0.0f,  1.0f) * distance_to_go;
					distance_to_go = 0;
				}
				
				break;
			case Direction::East:
				distance_to_next = ceil(position_.x) - position_.x;
				if(distance_to_next == 0)
				{
					distance_to_next = 1.0f;
					if(DoTurns())
						continue;
				}

				if(distance_to_go >= distance_to_next)
				{
					distance_to_go -= distance_to_next;
					position_ += Vector2f( 1.0f,  0.0f) * distance_to_next;
					DoTurns();
				} else
				{
					position_ += Vector2f( 1.0f,  0.0f) * distance_to_go;
					distance_to_go = 0;
				}
				
				break;
			case Direction::West:
				distance_to_next = position_.x - floor(position_.x);
				if(distance_to_next == 0)
				{
					distance_to_next = 1.0f;
					if(DoTurns())
						continue;
				}
				
				if(distance_to_go >= distance_to_next)
				{
					distance_to_go -= distance_to_next;
					position_ += Vector2f(-1.0f,  0.0f) * distance_to_next;
					DoTurns();
				} else
				{
					position_ += Vector2f(-1.0f,  0.0f) * distance_to_go;
					distance_to_go = 0;
				}
				
				break;
		}
	}
}

void Walker::Reset()
{
	position_ = initial_position_;
	direction_ = initial_direction_;
	death_time_ = 0;
	walker_state_ = WalkerState::Live;
}

void Walker::SetSpeed(float _speed)
{
	speed_ = _speed;
}

void Walker::Kill()
{
	walker_state_ = WalkerState::Killed;
}

WalkerState::Enum Walker::GetWalkerState()
{
	return walker_state_;
}

void Walker::SetEnemyType(EnemyTypes::EnemyType* _enemy_type)
{
	enemy_type_ = _enemy_type;
	speed_ = enemy_type_->speed;
	health_ = enemy_type_->health;
}