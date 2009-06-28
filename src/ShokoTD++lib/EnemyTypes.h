#pragma once
#include <map>
#include <string>
class Animation;

namespace EnemyTypes
{
	class EnemyType
	{
	public:
		float health;
		float speed;
		std::string name;
		Animation* directions[5];
		Animation* west_animation;
		Animation* east_animation;
		Animation* north_animation;
		Animation* south_animation;
		Animation* death_animation;
		Animation* stopped_animation;
	};

	extern std::map<std::string, EnemyType> EnemyMap;
	bool LoadEnemies();
	EnemyType* GetEnemyType(std::string);
	void TickEnemyAnimations(float _dt);
}
