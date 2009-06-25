#include "stdafx.h"
#include <World.h>

TEST(CheckWorldLoadsOK)
{
	World* pworld = new World("Level1.level");
	CHECK_EQUAL(0, pworld->GetEnemies().size());
	CHECK_EQUAL(Vector2i(12, 9), pworld->GetSize());

	CHECK_EQUAL(1, pworld->GetActiveWaves().size());
	CHECK_EQUAL(0, pworld->GetFinishedWaves().size());


	CHECK_EQUAL(2, pworld->GetSpawners().size());
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

TEST(CheckWavesSpawn)
{
	World* pworld = new World("Level1.level");

	CHECK_EQUAL(WorldState::OK, pworld->Tick(0));
	//First wave at 10s in, of 10 enemies at 0.2s each, so finish at 12s
	for(float t = 0; t < 10.99; t+= 0.01)
	{
		pworld->Tick(0.01);
	}
	
	// Two spawners at 5/s for 1s = 10
	CHECK_EQUAL(10, pworld->GetEnemies().size());

	delete pworld;
}
