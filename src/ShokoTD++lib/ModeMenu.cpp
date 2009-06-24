#include "ModeMenu.h"

IMode* ModeMenu::Teardown()
{
	return IMode::Teardown();
}

void ModeMenu::Setup()
{
}

bool ModeMenu::Tick(float _dt)
{
	return 0;
}

ModeType::Enum ModeMenu::GetType()
{
	return ModeType::Menu;
}