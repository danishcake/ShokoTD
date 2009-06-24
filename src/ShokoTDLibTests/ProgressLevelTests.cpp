#include "stdafx.h"
#include <ProgressLevel.h>
#include <GameReport.h>

TEST(ProgressLevelBasics)
{
	ProgressLevel pl("Level 1", "Level 1.Level");
	CHECK_EQUAL("Level 1", pl.GetName());
	CHECK_EQUAL("Level 1.Level", pl.GetFilename());
}

TEST(ProgressLevelUnlocks)
{
	ProgressLevel pl("Level 1", "Level 1.Level");
	std::vector<std::string> skills;
	skills.push_back("Fireball");
	pl.AddUnlock(true, false, false, std::vector<std::string>(), skills);
	GameReport gr;
	gr.SetAlignment(AlignmentVector(50, 5, 5));

	std::vector<std::string> unlocked_skills;
	std::vector<std::string> unlocked_levels;
	
	CHECK_EQUAL(0, unlocked_skills.size());
	CHECK_EQUAL(0, unlocked_levels.size());
	pl.GetRewards(gr, unlocked_levels, unlocked_skills);
	CHECK_EQUAL(1, unlocked_skills.size());
	CHECK_EQUAL(0, unlocked_levels.size());
	CHECK_EQUAL("Fireball", unlocked_skills.at(0));
}
