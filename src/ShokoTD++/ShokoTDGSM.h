#pragma once
#include <IMode.h>
struct SDL_Surface;

class ShokoTDGSM
{
private:
	IMode* game_mode_;
public:
	ShokoTDGSM(void);
	~ShokoTDGSM(void);

	void Init();
	/* Main tick routine */
	bool Tick(float _timespan); //Returns true when exitting
	void Draw(SDL_Surface* _target);
};
