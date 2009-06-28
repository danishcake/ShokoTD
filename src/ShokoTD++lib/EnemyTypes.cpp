#include "tinyxml.h"
#include "EnemyTypes.h"
#include "Animation.h"
#include "AnimationSet.h"
#include "TextureManager.h"
#include "Logger.h"
#include <boost/foreach.hpp>

namespace EnemyTypes
{
	std::map<std::string, EnemyType> EnemyMap;

	bool LoadEnemies()
	{
		bool error_found = false;
		TiXmlDocument doc("Enemies.xml");
		if(doc.LoadFile())
		{
			TiXmlElement* enemies_list = doc.FirstChildElement("Enemies");
			if(!enemies_list)
				Logger::ErrorOut() << "Enemies node in Enemies.xml does not exist\n";
			TiXmlElement* enemy = enemies_list->FirstChildElement("Enemy");
			while(enemy)
			{
				std::string name;
				std::string animation;
				float health = 100;
				float speed = 1;
				error_found |= (enemy->QueryValueAttribute("name", &name) != TIXML_SUCCESS);
				error_found |= (enemy->QueryValueAttribute("animation", &animation) != TIXML_SUCCESS);
				error_found |= (enemy->QueryValueAttribute("health", &health) != TIXML_SUCCESS);
				error_found |= (enemy->QueryValueAttribute("speed", &speed) != TIXML_SUCCESS);

				if(!error_found)
				{
					EnemyMap[name].health = health;
					EnemyMap[name].name = name;
					EnemyMap[name].speed = speed;
					AnimationSet* as = TextureManager::GetAnimationSet(animation);
					if(!as)
					{
						Logger::ErrorOut() << "Unable to load animationset \"" << animation << "\"\n";
						error_found = true;
					} else
					{
						EnemyMap[name].north_animation = as->GetAnimation("North");
						EnemyMap[name].south_animation = as->GetAnimation("South");
						EnemyMap[name].east_animation = as->GetAnimation("East");
						EnemyMap[name].west_animation = as->GetAnimation("West");
						EnemyMap[name].stopped_animation = as->GetAnimation("Stopped");
						EnemyMap[name].death_animation = as->GetAnimation("Death");
						EnemyMap[name].directions[0] = EnemyMap[name].north_animation;
						EnemyMap[name].directions[1] = EnemyMap[name].south_animation;
						EnemyMap[name].directions[2] = EnemyMap[name].east_animation;
						EnemyMap[name].directions[3] = EnemyMap[name].west_animation;
						EnemyMap[name].directions[4] = EnemyMap[name].stopped_animation;
						Logger::DiagnosticOut() << "Added enemy type: " << name << "\n";
					}
				}
				enemy = enemy->NextSiblingElement("Enemy");
			}
		} else
		{
			Logger::ErrorOut() << "Unable load Enemies.xml, so enemies will not be loaded\n";
			error_found = true;
		}
		return error_found;
	}

	EnemyType* GetEnemyType(std::string _enemy_type)
	{
		//This might move around if I add more elements?
		if(EnemyMap.find(_enemy_type) != EnemyMap.end())
		{
			return &EnemyMap[_enemy_type];
		}
		return NULL;
	}

	void TickEnemyAnimations(float _dt)
	{
		std::pair<std::string, EnemyType> pair;
		BOOST_FOREACH(pair, EnemyMap)
		{
			pair.second.north_animation->Tick(_dt);
			pair.second.south_animation->Tick(_dt);
			pair.second.east_animation->Tick(_dt);
			pair.second.west_animation->Tick(_dt);
			pair.second.stopped_animation->Tick(_dt);
			pair.second.death_animation->Tick(_dt);
		}
	}
}
