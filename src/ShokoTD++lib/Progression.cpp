#pragma once
#include "Progression.h"
#include "ProgressLevel.h"
#include "GameReport.h"
#include <TinyXML.h>
#include "Logger.h"
#include "Skill.h"


Progression::Progression(std::string _campaign, std::string _savefile)
{
	state_ = ProgressionState::OK;

	TiXmlDocument doc("Levels/" + _campaign);
	doc.LoadFile();
	if(!doc.Error())
	{
		if(!LoadCampaign(&doc))
			state_ = ProgressionState::LoadError;
	} else
	{
		Logger::ErrorOut() << "Unable to load campaign " << _campaign << "\n";
		Logger::ErrorOut() << "Error: " << doc.ErrorDesc() << "\n";
		state_ = ProgressionState::LoadError;
	}
}

bool Progression::LoadUnlockables(TiXmlElement* _first, ProgressLevel* _progress_level)
{
	bool error_found = false;
	TiXmlElement* unlockable = _first;
	while(unlockable)
	{
		bool require_good = false;
		bool require_neutral = false;
		bool require_evil = false;
		std::vector<std::string> skill_unlocks;
		std::vector<std::string> level_unlocks;

		TiXmlElement* condition = _first->FirstChildElement("Condition");
		while(condition)
		{
			std::string alignment;
			if(condition->QueryValueAttribute("alignment", &alignment))
			{
				if(alignment == "evil")
				{
					require_evil = true;
				} else if(alignment == "good")
				{
					require_good = true;
				} else if(alignment == "neutral")
				{
					require_neutral = true;
				}
			}
			condition = condition->NextSiblingElement("Condition");
		}
		
		TiXmlElement* level_unlock = _first->FirstChildElement("LevelUnlock");
		while(level_unlock)
		{
			std::string name;
			if(level_unlock->QueryValueAttribute("name", &name) != TIXML_SUCCESS)
			{
				error_found = true;
				Logger::ErrorOut() << "LevelUnlock requires a name attribute\n";
			} else
				level_unlocks.push_back(name);
			level_unlock = level_unlock->NextSiblingElement("LevelUnlock");
		}

		TiXmlElement* skill_unlock = _first->FirstChildElement("SkillUnlock");
		while(skill_unlock)
		{
			std::string name;
			if(skill_unlock->QueryValueAttribute("name", &name) != TIXML_SUCCESS)
			{
				error_found = true;
				Logger::ErrorOut() << "SkillUnlock requires a name attribute\n";
			} else
				skill_unlocks.push_back(name);
			skill_unlock = skill_unlock ->NextSiblingElement("SkillUnlock");
		}
		_progress_level->AddUnlock(require_good, require_neutral, require_evil, level_unlocks, skill_unlocks);
		
		unlockable = unlockable->NextSiblingElement("Unlockable");
	}
	return false;
}

bool Progression::LoadCampaign(TiXmlDocument* _doc)
{
	bool error_found = false;
	TiXmlElement* root = _doc->FirstChildElement("Campaign");
	if(root)
	{
		TiXmlElement* level = root->FirstChildElement("Level");
		while(level)
		{
			std::string levelname;
			std::string filename;
			bool locked = false;
			error_found |= level->QueryValueAttribute("name", &levelname) != TIXML_SUCCESS;
			error_found |= level->QueryValueAttribute("file", &filename) != TIXML_SUCCESS;
			error_found |= level->QueryValueAttribute("locked", &locked) != TIXML_SUCCESS;

			
			if(error_found)
				break;
			else
			{
				ProgressLevel* pl = new ProgressLevel(levelname, filename);
				pl->SetLocked(locked);
				LoadUnlockables(level->FirstChildElement("Unlockable"), pl);
				levels_.push_back(pl);
			}
			level = level->NextSiblingElement("Level");
		}

		TiXmlElement* skill = root->FirstChildElement("StartingSkill");
		while(skill)
		{
			bool purchased = false;
			std::string skillname;
			skill->QueryValueAttribute("name", &skillname);
			skill->QueryValueAttribute("purchased", &purchased);
			if(skill_manager_.SkillExists(skillname))
			{
				skill_manager_.Unlock(skillname);
				if(purchased)
					skill_manager_.Purchase(skillname);
			} else
			{
				Logger::DiagnosticOut() << "Skill " << skillname << " does not exist in skills.xml\n";
			}

			skill = skill->NextSiblingElement("StartingSkill");
		}
	} else
	{
		Logger::ErrorOut() << "Unable to find root Campaign node\n";
		error_found = true;
	}
	return !error_found;
}

Progression::~Progression()
{
	//TODO fix leak by deleting array contents
}

std::vector<ProgressLevel*> Progression::GetUnlocked()
{
	std::vector<ProgressLevel*> unlocked_levels;
	for(std::vector<ProgressLevel*>::iterator it = levels_.begin(); it != levels_.end(); ++it)
	{
		if(!(*it)->GetLocked())
			unlocked_levels.push_back(*it);
	}
	return unlocked_levels;
}

std::vector<ProgressLevel*> Progression::GetLocked()
{
	std::vector<ProgressLevel*> locked_levels;
	for(std::vector<ProgressLevel*>::iterator it = levels_.begin(); it != levels_.end(); ++it)
	{
		if((*it)->GetLocked())
			locked_levels.push_back(*it);
	}
	return locked_levels;
}

bool Progression::Has(std::string _level)
{
	for(std::vector<ProgressLevel*>::iterator it = levels_.begin(); it != levels_.end(); ++it)
	{
		if((*it)->GetName() == _level)
			return true;
	}
	return false;
}

bool Progression::IsUnlocked(std::string _level)
{
	for(std::vector<ProgressLevel*>::iterator it = levels_.begin(); it != levels_.end(); ++it)
	{
		if((*it)->GetName() == _level)
		{
			if(!(*it)->GetLocked())
				return true;
			else
				return false;
		}
	}
	return false;
}

bool Progression::IsBeaten(std::string _level)
{
	for(std::vector<ProgressLevel*>::iterator it = levels_.begin(); it != levels_.end(); ++it)
	{
		if((*it)->GetName() == _level)
		{
			if((*it)->GetBeaten())
				return true;
			else
				return false;
		}
	}
	return false;
}

void Progression::Unlock(std::string _level)
{
	for(std::vector<ProgressLevel*>::iterator it = levels_.begin(); it != levels_.end(); ++it)
	{
		if((*it)->GetName() == _level)
		{
			(*it)->SetLocked(false);
		}
	}
}
void Progression::GetRewards(std::string _level, std::vector<std::string>& _unlocked_skills, 
												 std::vector<std::string>& _locked_skills,
												 std::vector<std::string>& _unlocked_levels,
												 std::vector<std::string>& _locked_levels)
{
	for(std::vector<ProgressLevel*>::iterator it = levels_.begin(); it != levels_.end(); ++it)
	{
		if((*it)->GetName() == _level)
		{
			std::vector<std::string> reward_skills;
			std::vector<std::string> reward_levels;
			(*it)->GetUnlockables(reward_levels, reward_skills);
			for(std::vector<std::string>::iterator it = reward_levels.begin(); it != reward_levels.end(); ++it)
			{
				if(IsUnlocked(*it))
					_unlocked_levels.push_back(*it);
				else
					_locked_levels.push_back(*it);
			}
			for(std::vector<std::string>::iterator it = reward_skills.begin(); it != reward_skills.end(); ++it)
			{
				if(skill_manager_.SkillAvailable(*it))
					_unlocked_skills.push_back(*it);
				else
					_locked_skills.push_back(*it);
			}

		}
	}
}

std::string Progression::GetLevelFilename(std::string _name)
{
	for(std::vector<ProgressLevel*>::iterator it = levels_.begin(); it != levels_.end(); ++it)
	{
		if((*it)->GetName() == _name)
		{
			return (*it)->GetFilename();
		}
	}
	return "";
}

void Progression::ReportCompletion(std::string _level, GameReport _gr)
{
	for(std::vector<ProgressLevel*>::iterator it = levels_.begin(); it != levels_.end(); ++it)
	{
		if((*it)->GetName() == _level)
		{
			std::vector<std::string> reward_levels;
			std::vector<std::string> reward_skills;
			(*it)->SetBeaten(true);
			(*it)->GetRewards(_gr, reward_levels, reward_skills);
			
			for(std::vector<std::string>::iterator it2 = reward_levels.begin(); it2 != reward_levels.end(); ++it2)
			{
				Unlock(*it2);
			}
			for(std::vector<std::string>::iterator it2 = reward_skills.begin(); it2 != reward_skills.end(); ++it2)
			{
				skill_manager_.Unlock(*it2);
			}
		}
	}
}