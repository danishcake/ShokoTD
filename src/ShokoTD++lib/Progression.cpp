#pragma once
#include "Progression.h"
#include "ProgressLevel.h"
#include "GameReport.h"
#include <TinyXML.h>
#include "Logger.h"
#include "Skill.h"

bool Progression::cheat_ = false;

Progression::Progression(std::string _campaign, std::string _savefile)
{
	state_ = ProgressionState::OK;
	campaign_ = _campaign;
	savefile_ = _savefile;
	good_points_ = 10;
	evil_points_ = 10;


	TiXmlDocument doc("Levels/" + _campaign);
	doc.LoadFile();
	if(!doc.Error())
	{
		if(!LoadCampaign(&doc) || LoadSavefile())
			state_ = ProgressionState::LoadError;
	} else
	{
		Logger::ErrorOut() << "Unable to load campaign " << _campaign << "\n";
		Logger::ErrorOut() << "Error: " << doc.ErrorDesc() << "\n";
		state_ = ProgressionState::LoadError;
	}

	if(cheat_)
	{
		good_points_ = 999;
		evil_points_ = 999;
	}
	//TODO load progress
}

bool Progression::LoadSavefile()
{
	bool error = false;
	TiXmlDocument doc(savefile_);
	doc.LoadFile();
	if(!doc.Error())
	{
		TiXmlElement* root = doc.FirstChildElement("Progress");
		if(root)
		{
			TiXmlElement* points = root->FirstChildElement("Points");
			if(points)
			{
				int good = 0;
				int evil = 0;
				if(points->QueryValueAttribute("Good", &good) == TIXML_SUCCESS &&
				   points->QueryValueAttribute("Evil", &evil) == TIXML_SUCCESS)
				{
					good_points_ = good;
					evil_points_ = evil;
				} else error = true;
			} else error = true;

			TiXmlElement* levels = root->FirstChildElement("Levels");
			if(levels)
			{
				TiXmlElement* level = levels->FirstChildElement("Level");
				while(level)
				{
					std::string id;
					int beaten = 0;
					if(level->QueryValueAttribute("id", &id) == TIXML_SUCCESS &&
					   level->QueryIntAttribute("Beaten", &beaten) == TIXML_SUCCESS)
					{
						for(std::vector<ProgressLevel*>::iterator it = levels_.begin(); it != levels_.end(); ++it)
						{
							if((*it)->GetFilename() == id)
							{
								(*it)->SetLocked(false);
								(*it)->SetBeaten(beaten ? 1 : 0);
							}
						}
					} else
					{
						error = true;
						break;
					}

					level = level->NextSiblingElement("Level");
				}
			} else error = true;
			
			TiXmlElement* skills = root->FirstChildElement("Skills");
			if(skills)
			{
				TiXmlElement* skill = skills->FirstChildElement("Skill");
				while(skill)
				{
					std::string id;
					int purchased = 0;
					if(skill->QueryValueAttribute("id", &id) == TIXML_SUCCESS &&
					   skill->QueryIntAttribute("Purchased", &purchased) == TIXML_SUCCESS)
					{
						std::vector<Skill*> sm_skills = skill_manager_.GetAllSkills();
						for(std::vector<Skill*>::iterator it = sm_skills.begin(); it != sm_skills.end(); ++it)
						{
							if((*it)->GetName() == id)
							{
								(*it)->SetUnlocked(true);
								(*it)->SetPurchased(purchased ? 1 : 0);
								TiXmlElement* rank = skill->FirstChildElement("Rank");
								while(rank)
								{
									std::string rid;
									int level = 0;
									if(rank->QueryValueAttribute("id", &rid) == TIXML_SUCCESS &&
									   rank->QueryIntAttribute("Level", &level) == TIXML_SUCCESS)
									{
										SkillLevel* sl = (*it)->GetSkillLevel(rid);
										if(sl)
										{
											sl->SetLevel(level);
										} else
											Logger::DiagnosticOut() << "Malformed skill in campaign savefile\n";
									} else Logger::DiagnosticOut() << "Malformed skill in campaign savefile\n";
									rank = rank->NextSiblingElement("Rank");
								}

							}
						}
					} else
					{
						error = true;
						break;
					}

					skill = skill->NextSiblingElement("Skill");
				}
			} else error = true;
		}
	}
	if(error)
		Logger::ErrorOut() << "Malformed savefile\n";
	return error;
}

bool Progression::LoadUnlockables(TiXmlElement* _first, ProgressLevel* _progress_level)
{
	bool error_found = false;
	TiXmlElement* unlockable = _first;
	while(unlockable)
	{
		bool require_good = false;
		bool require_evil = false;
		bool require_flawless = false;
		std::vector<std::string> skill_unlocks;
		std::vector<std::string> level_unlocks;

		TiXmlElement* condition = unlockable->FirstChildElement("Condition");
		while(condition)
		{
			std::string alignment;
			if(condition->QueryValueAttribute("alignment", &alignment) == TIXML_SUCCESS)
			{
				if(alignment == "Evil")
				{
					require_evil = true;
				} else if(alignment == "Good")
				{
					require_good = true;
				}
			}
			condition->QueryValueAttribute("flawless", &require_flawless);
			condition = condition->NextSiblingElement("Condition");
		}
		
		TiXmlElement* level_unlock = unlockable->FirstChildElement("LevelUnlock");
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

		TiXmlElement* skill_unlock = unlockable->FirstChildElement("SkillUnlock");
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
		_progress_level->AddUnlock(require_good, require_evil, require_flawless, level_unlocks, skill_unlocks);
		
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
				if(cheat_)
					pl->SetLocked(false);
				else
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
	//TODO save progress
	TiXmlDocument progress(savefile_);
	TiXmlDeclaration* decl = new TiXmlDeclaration();
	progress.LinkEndChild(decl);

	TiXmlElement* pSaveEl = new TiXmlElement("Progress");
	pSaveEl->SetAttribute("Campaign", campaign_);
	
	TiXmlElement* pPoints = new TiXmlElement("Points");
	pPoints->SetAttribute("Good", good_points_);
	pPoints->SetAttribute("Evil", evil_points_);
	
	TiXmlElement* pLevels = new TiXmlElement("Levels");
	TiXmlElement* pSkills = new TiXmlElement("Skills");
	
	for(std::vector<ProgressLevel*>::iterator it = levels_.begin(); it != levels_.end(); ++it)
	{
		if((*it)->GetBeaten())
		{
			TiXmlElement* pBeaten = new TiXmlElement("Level");
			pBeaten->SetAttribute("id", (*it)->GetFilename());
			pBeaten->SetAttribute("Beaten", 1);
			pLevels->LinkEndChild(pBeaten);
		} else if(!(*it)->GetLocked())
		{
			TiXmlElement* pUnlocked = new TiXmlElement("Level");
			pUnlocked->SetAttribute("id", (*it)->GetFilename());
			pUnlocked->SetAttribute("Beaten", 0);
			pLevels->LinkEndChild(pUnlocked);
		}
	}
	std::vector<Skill*> skills = skill_manager_.GetAllSkills();
	for(std::vector<Skill*>::iterator it = skills.begin(); it != skills.end(); ++it)
	{
		if((*it)->GetPurchased())
		{
			TiXmlElement* pPurchased = new TiXmlElement("Skill");
			pPurchased->SetAttribute("id", (*it)->GetName());
			pPurchased->SetAttribute("Purchased", 1);
			std::vector<SkillLevel> skill_levels = (*it)->GetUpgrades();
			for(std::vector<SkillLevel>::iterator it = skill_levels.begin(); it != skill_levels.end(); ++it)
			{
				TiXmlElement* pRank = new TiXmlElement("Rank");
				pRank->SetAttribute("id", it->GetName());
				pRank->SetAttribute("Level", it->GetLevel());
				pPurchased->LinkEndChild(pRank);
			}

			pSkills->LinkEndChild(pPurchased);
		} else if((*it)->GetUnlocked())
		{
			TiXmlElement* pUnlocked = new TiXmlElement("Skill");
			pUnlocked->SetAttribute("id", (*it)->GetName());
			pUnlocked->SetAttribute("Purchased", 0);
			pSkills->LinkEndChild(pUnlocked);
		}
	}

	progress.LinkEndChild(pSaveEl);
	pSaveEl->LinkEndChild(pPoints);
	pSaveEl->LinkEndChild(pLevels);
	pSaveEl->LinkEndChild(pSkills);

	if(!progress.SaveFile())
	{
		Logger::ErrorOut() << "Unable to save progress to " << savefile_ << " - perhaps write access was not granted?\n";
	}
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
	good_points_ += _gr.GetAlignment().GetGoodScore();
	evil_points_ += _gr.GetAlignment().GetEvilScore();
}

std::string Progression::GetCampaignIcon(std::string _campaign)
{
	std::string icon_filename = "CampaignError.png";
	TiXmlDocument doc("Levels/" + _campaign + ".Campaign");
	doc.LoadFile();
	if(!doc.Error())
	{
		TiXmlElement* campaign_el = doc.FirstChildElement("Campaign");
		if(campaign_el)
		{
			std::string attr;
			if(campaign_el->QueryValueAttribute("icon", &attr) == TIXML_SUCCESS)
			{
				icon_filename = attr;
			}
		}

	}
	return icon_filename;
}