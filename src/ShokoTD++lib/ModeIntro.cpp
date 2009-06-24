#include "ModeIntro.h"
#include "ModeMenu.h"
#include <Widget>

IMode* ModeIntro::Teardown()
{
	return IMode::Teardown();
}

void ModeIntro::Setup()
{
}

bool ModeIntro::Tick(float _dt)
{
	return 0;
}

ModeType::Enum ModeIntro::GetType()
{
	return ModeType::Intro;
}