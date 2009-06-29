#include "ProgressLevel.h"
#include "GameReport.h"
#include <algorithm>

ProgressLevel::ProgressLevel(std::string _name, std::string _filename) :
name_(_name), filename_(_filename)
{

}

void ProgressLevel::AddUnlock(bool _good, bool _neutral, bool _evil, std::vector<std::string> _levels, std::vector<std::string> _skills)
{
	Unlockable ul;
	ul.condition.good = _good;
	ul.condition.neutral = _neutral;
	ul.condition.evil = _evil;

	ul.reward.levels = _levels;
	ul.reward.skills = _skills;
	unlockables_.push_back(ul);
}

void ProgressLevel::GetRewards(GameReport _gr, std::vector<std::string> &_levels, std::vector<std::string>& _skills)
{
	bool meet_evil = (float)_gr.GetAlignment().GetEvil() / (float)_gr.GetAlignment().GetSum() > 0.8f;
	bool meet_good = (float)_gr.GetAlignment().GetGood() / (float)_gr.GetAlignment().GetSum() > 0.8f;
	bool meet_neutral = (float)_gr.GetAlignment().GetNeutral() / (float)_gr.GetAlignment().GetSum() > 0.8f;

	std::vector<std::string> skills_to_unlock;
	std::vector<std::string> levels_to_unlock;
	for(std::vector<Unlockable>::iterator it = unlockables_.begin(); it != unlockables_.end(); ++it)
	{
		if((!it->condition.evil    || (it->condition.evil && meet_evil)) &&
		   (!it->condition.good    || (it->condition.good && meet_good)) &&
		   (!it->condition.neutral || (it->condition.neutral&& meet_neutral)))
		{
			for(std::vector<std::string>::iterator it2 = it->reward.levels.begin(); it2 != it->reward.levels.end(); ++it2)
			{
				if(std::find(levels_to_unlock.begin(), levels_to_unlock.end(), *it2) == levels_to_unlock.end())
				{
					levels_to_unlock.push_back(*it2);
				}
			}
			for(std::vector<std::string>::iterator it2 = it->reward.skills.begin(); it2 != it->reward.skills.end(); ++it2)
			{
				if(std::find(skills_to_unlock.begin(), skills_to_unlock.end(), *it2) == skills_to_unlock.end())
				{
					skills_to_unlock.push_back(*it2);
				}
			}
		}
	}

	_levels = levels_to_unlock;
	_skills = skills_to_unlock;
}

void ProgressLevel::GetUnlockables(std::vector<std::string> &_levels, std::vector<std::string>& _skills)
{
	std::vector<std::string> skills_to_unlock;
	std::vector<std::string> levels_to_unlock;
	for(std::vector<Unlockable>::iterator it = unlockables_.begin(); it != unlockables_.end(); ++it)
	{
		for(std::vector<std::string>::iterator it2 = it->reward.levels.begin(); it2 != it->reward.levels.end(); ++it2)
		{
			if(std::find(levels_to_unlock.begin(), levels_to_unlock.end(), *it2) == levels_to_unlock.end())
			{
				levels_to_unlock.push_back(*it2);
			}
		}
		for(std::vector<std::string>::iterator it2 = it->reward.skills.begin(); it2 != it->reward.skills.end(); ++it2)
		{
			if(std::find(skills_to_unlock.begin(), skills_to_unlock.end(), *it2) == skills_to_unlock.end())
			{
				skills_to_unlock.push_back(*it2);
			}
		}
	}
	_levels = levels_to_unlock;
	_skills = skills_to_unlock;
}
