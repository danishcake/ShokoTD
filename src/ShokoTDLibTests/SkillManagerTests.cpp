#include "stdafx.h"
#include <SkillManager.h>

TEST(SkillManagerBasics)
{
	SkillManager sm;
	CHECK(!sm.GetError());

	CHECK_EQUAL(2, sm.GetAllSkills().size());
	sm.Unlock("Arrows");
	sm.Purchase("Arrows");
}

