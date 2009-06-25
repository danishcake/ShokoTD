#include "stdafx.h"
#include <World.h>

TEST(CheckWorldLoadsOK)
{
	World* pworld = new World("Level1.level");
	CHECK_EQUAL(0, pworld->GetEnemies().size());
	CHECK_EQUAL(Vector2i(12, 9), pworld->GetSize());

	CHECK_EQUAL(1, pworld->GetActiveWaves().size());
	CHECK_EQUAL(0, pworld->GetFinishedWaves().size());
	delete pworld;
}

TEST(CheckWavesFinish)
{
	World* pworld = new World("Level1.level");
	CHECK_EQUAL(0, pworld->GetEnemies().size());
	CHECK_EQUAL(Vector2i(12, 9), pworld->GetSize());

	CHECK_EQUAL(1, pworld->GetActiveWaves().size());

	//First wave at 10s in, of 10 enemies at 0.2s each, so finish at 12s
	for(float t = 0; t < 13; t+= 0.01666)
	{
		pworld->Tick(0.01666);
	}
	CHECK_EQUAL(0, pworld->GetActiveWaves().size());
	CHECK_EQUAL(1, pworld->GetFinishedWaves().size());
	delete pworld;
}