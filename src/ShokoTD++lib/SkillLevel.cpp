#include "SkillLevel.h"

SkillLevel::SkillLevel(std::string _level_name)
{
	level_name_ = _level_name;
	max_level_ = 1;
	level_ = 1;
}

unsigned int SkillLevel::GetUpgradeCost()
{
	return rank_costs_[level_ - 1];
}

void SkillLevel::AddRank(unsigned int _cost)
{
	rank_costs_.push_back(_cost);
	max_level_++;
}

void SkillLevel::Upgrade()
{
	if(level_ < max_level_)
		level_++;
}
