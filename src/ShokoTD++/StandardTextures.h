#pragma once
#include <vmath.h>
#include <boost/shared_ptr.hpp>
class Animation;
class World;

namespace StandardTextures
{
	extern Animation* cat_animations[5]; //Each direction
	extern Animation* cat_death;
	extern Animation* arrows[5];
	extern Animation* half_arrows[5];
	extern Animation* rocket_normal_animation;
	extern Animation* hole_animation;
	extern Animation* ring_animation;
	extern Animation* tile_a_animation;
	extern Animation* tile_b_animation;	
	extern Animation* wall_horz_animation;
	extern Animation* wall_vert_animation;


	void LoadTextures();
	void TickAnimations(float _dt);
};
