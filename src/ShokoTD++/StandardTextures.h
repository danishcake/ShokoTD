#pragma once
#include <vmath.h>
#include <boost/shared_ptr.hpp>
class Animation;
class World;

namespace StandardTextures
{
	extern Animation* arrows[5];
	extern Animation* half_arrows[5];
	extern Animation* rocket_normal_animation;
	extern Animation* hole_animation;
	extern Animation* cross_animation;
	extern Animation* ring_animation;
	extern Animation* tile_a_animation;
	extern Animation* tile_b_animation;	
	extern Animation* wall_horz_animation;
	extern Animation* wall_vert_animation;
	extern Animation* grid_animation;
	extern Animation* spawner_animation;
	extern Animation* burning_animation;
	extern Animation* slow_animation;
	extern Animation* craze_animation;
	extern Animation* burning_animation;
	extern Animation* healthbar_animation;

	void LoadTextures();
	void TickAnimations(float _dt);
};
