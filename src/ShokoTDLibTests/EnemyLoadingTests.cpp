#include "stdafx.h"
#include <EnemyTypes.h>

TEST(CheckEnemyLoading)
{
	CHECK_EQUAL(false, EnemyTypes::LoadEnemies());
	CHECK(EnemyTypes::EnemyMap.find("Swarmer") != EnemyTypes::EnemyMap.end());

}