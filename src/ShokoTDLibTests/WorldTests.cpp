#include "stdafx.h"
#include <World.h>

TEST(CheckWorldLoadsOK)
{
	World* pworld = new World("Level1.level");
	CHECK_EQUAL(0, pworld->GetEnemies().size());
	CHECK_EQUAL(Vector2i(12, 9), pworld->GetSize());
	delete pworld;
}
