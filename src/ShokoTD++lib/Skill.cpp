#include "Skill.h"

Skill::Skill(void)
{
	unlocked_ = false;
	purchased_ = false;

}

Skill::~Skill(void)
{
}

/* This is undoubtably a bad idea, as vector could reallocate itself. Will do, as vector is not changed */
SkillLevel* Skill::GetSkillLevel(std::string _name)
{
	for(std::vector<SkillLevel>::iterator it = upgrades_.begin(); it != upgrades_.end(); ++it)
	{
		if(it->GetName() == _name)
		{
			return &(*it);
		}
	}
}