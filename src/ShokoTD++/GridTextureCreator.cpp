#include "GridTextureCreator.h"
#include "SDLAnimationFrame.h"
#include <Animation.h>
#include "Settings.h"
#include <sdl.h>
#include "StandardTextures.h"

namespace
{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	Uint32 rmask = 0xff000000;
	Uint32 gmask = 0x00ff0000;
	Uint32 bmask = 0x0000ff00;
	Uint32 amask = 0x000000ff;
#else
	Uint32 rmask = 0x000000ff;
	Uint32 gmask = 0x0000ff00;
	Uint32 bmask = 0x00ff0000;
	Uint32 amask = 0xff000000;
#endif
}

Animation* CreateGridTexture(World* _world, Vector2i _grid_size)
{
	bool fill = true;

	Vector2i offset(2, 2);
	
	Vector2i size = _grid_size * _world->GetSize() + Vector2i(4, 4);
	SDL_Surface* p_checkerboard = SDL_CreateRGBSurface(SDL_SWSURFACE, size.x, size.y, 32, rmask, gmask, bmask, amask);
	SDL_Surface* old_checkboard = p_checkerboard;
	p_checkerboard = SDL_DisplayFormatAlpha(p_checkerboard);
	SDL_FreeSurface(old_checkboard);

	SDL_SetAlpha(p_checkerboard, 0, 255);
	SDL_FillRect(p_checkerboard, NULL, SDL_MapRGB(p_checkerboard->format, 255, 255, 255));

	SDL_Surface* old_screen = SDLAnimationFrame::screen_;
	SDLAnimationFrame::screen_ = p_checkerboard;

	for(int y = 0; y < _world->GetSize().y; y++)
	{
		fill = y % 2 == 0;
		for(int x = 0; x < _world->GetSize().x; x++)
		{
			fill = !fill;
			if(fill)
				StandardTextures::tile_a_animation->GetCurrentFrame()->Draw(offset + Vector2f(static_cast<float>(x * _grid_size.x), static_cast<float>(y * _grid_size.y)));
			else
				StandardTextures::tile_b_animation->GetCurrentFrame()->Draw(offset + Vector2f(static_cast<float>(x * _grid_size.x), static_cast<float>(y * _grid_size.y)));
		}
	}
	for(int y = 0; y < _world->GetSize().y; y++)
	{
		for(int x = 0; x < _world->GetSize().x; x++)
		{
			if(_world->GetGridSquare(Vector2i(x,y)).GetNorth())
			{
				StandardTextures::wall_horz_animation->GetCurrentFrame()->Draw(Vector2f(static_cast<float>(x * _grid_size.x), static_cast<float>(y * _grid_size.y)));
				if(y == 0)
				{
					StandardTextures::wall_horz_animation->GetCurrentFrame()->Draw(Vector2f(static_cast<float>(x * _grid_size.x), static_cast<float>(_world->GetSize().y * _grid_size.y)));
				}
			}
			if(_world->GetGridSquare(Vector2i(x,y)).GetWest())
			{
				StandardTextures::wall_vert_animation->GetCurrentFrame()->Draw(Vector2f(static_cast<float>(x * _grid_size.x), static_cast<float>(y * _grid_size.y)));
				if(x == 0)
				{
					StandardTextures::wall_vert_animation->GetCurrentFrame()->Draw(Vector2f(static_cast<float>(_world->GetSize().x * _grid_size.x), static_cast<float>(y * _grid_size.y)));
				}
			}
		}
	}


	SDLAnimationFrame::screen_ = old_screen;
/*
	for(int y = 0; y < _world->GetSize().y; y++)
	{
		fill = y % 2 == 0;
		for(int x = 0; x < _world->GetSize().x; x++)
		{
			SDL_Rect area;
			area.x = x * _grid_size.x;
			area.y = y * _grid_size.y;
			area.w = _grid_size.x;
			area.h = _grid_size.y;
			if(fill)
				SDL_FillRect(p_checkerboard, &area, SDL_MapRGB(p_checkerboard->format, Settings::GetGridColorA().r, Settings::GetGridColorA().g, Settings::GetGridColorA().b));
			else
				SDL_FillRect(p_checkerboard, &area, SDL_MapRGB(p_checkerboard->format, Settings::GetGridColorB().r, Settings::GetGridColorB().g, Settings::GetGridColorB().b));

			fill = !fill;
		}
	}
	for(int y = 0; y < _world->GetSize().y; y++)
	{
		for(int x = 0; x < _world->GetSize().x; x++)
		{
			if(_world->GetGridSquare(Vector2i(x,y)).GetNorth())
			{
				SDL_Rect area;
				area.x = x * _grid_size.x - 1;
				area.y = y * _grid_size.y - 1;
				area.w = _grid_size.x + 2;
				area.h = 2;
				SDL_FillRect(p_checkerboard, &area, SDL_MapRGB(p_checkerboard->format, 255, 0, 0));
				if(y == 0)
				{
					area.y = _grid_size.y * _world->GetSize().y;
					SDL_FillRect(p_checkerboard, &area, SDL_MapRGB(p_checkerboard->format, 255, 0, 0));
				}
			}
			if(_world->GetGridSquare(Vector2i(x,y)).GetWest())
			{
				SDL_Rect area;
				area.x = x * _grid_size.x - 1;
				area.y = y * _grid_size.y - 1;
				area.w = 2;
				area.h = _grid_size.y + 2;
				SDL_FillRect(p_checkerboard, &area, SDL_MapRGB(p_checkerboard->format, 255, 0, 0));
				if(x == 0)
				{
					area.x = _grid_size.x * _world->GetSize().x;
					SDL_FillRect(p_checkerboard, &area, SDL_MapRGB(p_checkerboard->format, 255, 0, 0));
				}
			}
		}
	}*/
	Animation* g_animation = new Animation();
	SDLAnimationFrame* af = new SDLAnimationFrame(0, 0, Vector2i(0, 0), p_checkerboard);
	g_animation->AddFrame(af);
	return g_animation;
	
}