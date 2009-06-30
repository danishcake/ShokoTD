#include "StandardTextures.h"
#include <AnimationSet.h>
//#include <Animation.h>
#include "SDLTextureManager.h"
#include <Direction.h>
#include <Logger.h>
#include <Animation.h>
#include <World.h>
#include <SDL.h>
#include "Settings.h"
#include "SDLAnimationFrame.h"

namespace StandardTextures
{
	Animation* arrows[5] = {0, 0, 0, 0, 0};
	Animation* half_arrows[5] = {0, 0, 0, 0, 0};
	Animation* rocket_normal_animation = 0;
	Animation* hole_animation = 0;
	Animation* cross_animation = 0;
	Animation* ring_animation = 0;
	Animation* grid_animation = 0;
	Animation* tile_a_animation = 0;
	Animation* tile_b_animation = 0;
	Animation* wall_horz_animation = 0;
	Animation* wall_vert_animation = 0;
	Animation* spawner_animation = 0;
	Animation* burning_animation = 0;
	Animation* slow_animation = 0;
	Animation* craze_animation = 0;

	void LoadTextures()
	{
		//Free textures if already loaded TODO

		//Acquire textures
		AnimationSet* tile_animation_set = SDLTextureManager::GetAnimationSet("Tiles.animation");
		if(tile_animation_set)
		{
			tile_a_animation =  tile_animation_set->GetAnimation("TileA");
			tile_b_animation =  tile_animation_set->GetAnimation("TileB");
			if(!tile_a_animation || !tile_b_animation)
			{
				Logger::ErrorOut() << "Unable to load either TileA or TileB\n";
			}
		} else
		{
			Logger::ErrorOut() << "Unable to load Tile animations\n";
		}

		AnimationSet* wall_animation_set = SDLTextureManager::GetAnimationSet("Walls.animation");
		if(wall_animation_set)
		{
			wall_horz_animation =  wall_animation_set->GetAnimation("Horizontal");
			wall_vert_animation =  wall_animation_set->GetAnimation("Vertical");
			if(!wall_horz_animation || !wall_vert_animation)
			{
				Logger::ErrorOut() << "Unable to load either horizontal or vertical walls\n";
			}
		} else
		{
			Logger::ErrorOut() << "Unable to load Wall animations\n";
		}

		AnimationSet* spawner_animation_set = SDLTextureManager::GetAnimationSet("FlyingSaucer.animation");
		if(spawner_animation_set)
		{
			spawner_animation =  spawner_animation_set->GetAnimation("Wobble");
			if(!spawner_animation)
			{
				Logger::ErrorOut() << "Unable to load spawner animation 'Wobble'\n";
			}
		} else
		{
			Logger::ErrorOut() << "Unable to load FlyingSaucer animations\n";
		}

		AnimationSet* burning_animation_set = SDLTextureManager::GetAnimationSet("Burning.animation");
		if(burning_animation_set)
		{
			burning_animation =  burning_animation_set->GetAnimation("Burn");
			if(!burning_animation)
			{
				Logger::ErrorOut() << "Unable to load burning animation 'Burn'\n";
			}
		} else
		{
			Logger::ErrorOut() << "Unable to load Burning animations\n";
		}

		AnimationSet* craze_animation_set = SDLTextureManager::GetAnimationSet("Craze.animation");
		if(craze_animation_set)
		{
			craze_animation =  craze_animation_set->GetAnimation("Craze");
			if(!craze_animation)
			{
				Logger::ErrorOut() << "Unable to load craze animation 'Craze'\n";
			}
		} else
		{
			Logger::ErrorOut() << "Unable to load Craze animations\n";
		}

		AnimationSet* slow_animation_set = SDLTextureManager::GetAnimationSet("Slow.animation");
		if(slow_animation_set)
		{
			slow_animation =  slow_animation_set->GetAnimation("Slow");
			if(!slow_animation)
			{
				Logger::ErrorOut() << "Unable to load slow animation 'Slow'\n";
			}
		} else
		{
			Logger::ErrorOut() << "Unable to load Slow animations\n";
		}


		AnimationSet* arrow_animation_set = SDLTextureManager::GetAnimationSet(Settings::GetArrowsSprite());
		if(arrow_animation_set)
		{
			for(int i = 0; i < 5; i++)
			{
				arrows[i] = arrow_animation_set->GetAnimation(Direction::ToString((Direction::Enum)i));
				if(!arrows[i])
				{
					Logger::ErrorOut() << "Unable to find direction "  << Direction::ToString((Direction::Enum)i) << " in Arrows.animation\n";
				}
			}
		} else
		{
			Logger::ErrorOut() << "Unable to load Arrow animations\n";
		}

		AnimationSet* half_arrow_animation_set = SDLTextureManager::GetAnimationSet(Settings::GetHalfArrowsSprite());
		if(half_arrow_animation_set)
		{
			for(int i = 0; i < 5; i++)
			{
				half_arrows[i] = half_arrow_animation_set->GetAnimation(Direction::ToString((Direction::Enum)i));
				if(!half_arrows[i])
				{
					Logger::ErrorOut() << "Unable to find direction "  << Direction::ToString((Direction::Enum)i) << " in HalfArrows.animation\n";
				}
			}
		} else
		{
			Logger::ErrorOut() << "Unable to load Half Arrow animations\n";
		}

		hole_animation = SDLTextureManager::GetAnimation(Settings::GetHoleSprite());
		cross_animation = SDLTextureManager::GetAnimation("Cross.animation");
		ring_animation = SDLTextureManager::GetAnimation(Settings::GetRingSprite());
		AnimationSet* rocket_animation_set = SDLTextureManager::GetAnimationSet(Settings::GetRocketSprite());
		rocket_normal_animation = rocket_animation_set->GetAnimation("Normal");


		if(!hole_animation)
		{
			Logger::ErrorOut() << "Unable to load hole animation\n";
		}
		
		if(!cross_animation)
		{
			Logger::ErrorOut() << "Unable to load cross animation\n";
		}

		if(!ring_animation)
		{
			Logger::ErrorOut() << "Unable to load ring animation\n";
		}

		if(!rocket_normal_animation)
		{
			Logger::ErrorOut() << "Unable to load rocket animation\n";
		}
	}

	void TickAnimations(float _dt)
	{
		hole_animation->Tick(_dt);
		cross_animation->Tick(_dt);
		rocket_normal_animation->Tick(_dt);
		spawner_animation->Tick(_dt);
		burning_animation->Tick(_dt);
		craze_animation->Tick(_dt);
		slow_animation->Tick(_dt);
	}
}