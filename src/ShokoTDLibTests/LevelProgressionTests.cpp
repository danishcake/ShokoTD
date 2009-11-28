#include "stdafx.h"
#include <Progression.h>
#include <ProgressLevel.h>
#include <AlignmentVector.h>
#include <Skills.h>

TEST(CheckProgressionBasics)
{
	//Loads from a file, queries a bit, unlocks, then queries to check it worked
	Progression progression("TestCampaign.Campaign", "", false);
	CHECK(!progression.GetError());
	CHECK_EQUAL(1, progression.GetUnlocked().size());
	CHECK_EQUAL(2, progression.GetLocked().size());
	CHECK_EQUAL(true, progression.Has("Level 1"));
	CHECK_EQUAL(true, progression.Has("Level 2"));
	CHECK_EQUAL(true, progression.Has("Level 3"));
	CHECK_EQUAL(true, progression.IsUnlocked("Level 1"));

	progression.Unlock("Level 2");
	CHECK_EQUAL(2, progression.GetUnlocked().size());
}

TEST(SkillUnlocks)
{
	Progression progression("TestCampaign.Campaign", "", false);
	CHECK(!progression.GetError());
	CHECK_EQUAL(2, progression.GetSkillsManager().GetUnlockedSkills().size());
	CHECK_EQUAL(1, progression.GetSkillsManager().GetPurchasedSkills().size());
	CHECK_EQUAL(1, progression.GetSkillsManager().GetAvailableSkills().size());
}

TEST(SkillUpgrades)
{
	Progression progression("TestCampaign.Campaign", "", false);
	CHECK(!progression.GetError());
	Skill* arrow_skill = progression.GetSkillsManager().GetSkill("Arrows");
	CHECK(arrow_skill);
	CHECK_EQUAL(3, arrow_skill->GetUpgrades().size());


}

TEST(CheckProgressionIsLoadable)
{
	
}

TEST(CheckProgressionIsSaveable)
{
	
}