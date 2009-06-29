#include "SkillManager.h"
#include "Skill.h"
#include <TinyXML.h>
#include "Logger.h"
#include <boost/lexical_cast.hpp>

SkillManager::SkillManager(void)
{
	error_ = false;
	/* Populate upgrade paths, costs etc*/
	TiXmlDocument doc("Skills.xml");
	doc.LoadFile();
	if(doc.Error())
	{
		Logger::ErrorOut() << "Unable to load skills.xml\n";
		Logger::ErrorOut() << "Error: " << doc.ErrorDesc() << "\n";
	}

	TiXmlElement* root = doc.FirstChildElement("Skills");
	if(root)
	{
		TiXmlElement* skill = root->FirstChildElement("Skill");
		while(skill)
		{
			std::vector<SkillLevel> upgrades;
			std::string name;
			std::string alignment;
			unsigned int init_cost = 0;
			error_ |= (skill->QueryValueAttribute("name", &name) != TIXML_SUCCESS);
			error_ |= (skill->QueryValueAttribute("alignment", &alignment)  != TIXML_SUCCESS);


			TiXmlElement* initialcost = skill->FirstChildElement("InitialCost");
			if(initialcost)
			{
				std::string init_cost_str = initialcost->GetText();
				init_cost = boost::lexical_cast<unsigned int, std::string>(init_cost_str);
			} else
			{
				Logger::ErrorOut() << "No initial cost for skill " << name << "\n";
				error_ = true;
			}

			//TODO upgrades
			TiXmlElement* upgrade = skill->FirstChildElement("Upgrade");
			while(upgrade)
			{
				std::string upgrade_name;
				upgrade->QueryValueAttribute("name", &upgrade_name);
				SkillLevel sl(upgrade_name);

				int ltv_rank = 1;
				TiXmlElement* level = upgrade->FirstChildElement("Level");
				while(level)
				{
					int rank = 0;
					int cost = 0;
					error_ |= (level->QueryIntAttribute("rank", &rank) != TIXML_SUCCESS);
					error_ |= (level->QueryIntAttribute("cost", &cost) != TIXML_SUCCESS);
					error_ |= (rank != ltv_rank + 1);
					ltv_rank = rank;
					sl.AddRank(cost);
					level = level->NextSiblingElement("Level");
				}
				upgrades.push_back(sl);
				upgrade = upgrade->NextSiblingElement("Upgrade");
			}

			if(error_)
				break;
			else
			{
				Skill* sk = new Skill();
				sk->SetCost(init_cost);
				sk->SetName(name);
				for(std::vector<SkillLevel>::iterator it = upgrades.begin(); it != upgrades.end(); ++it)
				{
					sk->AddUpgrade(*it);
				}
				if(alignment == "good")
					sk->SetAlignment(SkillAlignment::Good);
				else if(alignment == "evil")
					sk->SetAlignment(SkillAlignment::Evil);
				else if(alignment == "neutral")
					sk->SetAlignment(SkillAlignment::Neutral);
				else
				{
					Logger::ErrorOut() << "Skill alignment can only be 'good', 'evil' or 'neutral'\n";
					error_ = true;
					break;
				}
				skills_.push_back(sk);
			}

			skill = skill->NextSiblingElement("Skill");
		}
	} else
	{
		Logger::ErrorOut() << "Malformed skills.xml, no root element 'Skills'\n";
		error_ = true;
	}
}

SkillManager::~SkillManager(void)
{
}

void SkillManager::Unlock(std::string _skill)
{
	for(std::vector<Skill*>::iterator it = skills_.begin(); it != skills_.end(); ++it)
	{
		if((*it)->GetName() == _skill)
		{
			(*it)->SetUnlocked(true);
			break;
		}
	}
}

void SkillManager::Purchase(std::string _skill)
{
	for(std::vector<Skill*>::iterator it = skills_.begin(); it != skills_.end(); ++it)
	{
		if((*it)->GetName() == _skill)
		{
			(*it)->SetPurchased(true);
			break;
		}
	}
}

bool SkillManager::SkillExists(std::string _skill)
{
	for(std::vector<Skill*>::iterator it = skills_.begin(); it != skills_.end(); ++it)
	{
		if((*it)->GetName() == _skill)
		{
			return true;
		}
	}
	return false;
}

bool SkillManager::SkillAvailable(std::string _skill)
{
	for(std::vector<Skill*>::iterator it = skills_.begin(); it != skills_.end(); ++it)
	{
		if((*it)->GetName() == _skill)
		{
			return (*it)->GetUnlocked();
		}
	}
	return false;
}

bool SkillManager::SkillPurchased(std::string _skill)
{
	for(std::vector<Skill*>::iterator it = skills_.begin(); it != skills_.end(); ++it)
	{
		if((*it)->GetName() == _skill)
		{
			return (*it)->GetPurchased();
		}
	}
	return false;
}

std::vector<Skill*> SkillManager::GetAvailableSkills()
{
	std::vector<Skill*> skills;
	for(std::vector<Skill*>::iterator it = skills_.begin(); it != skills_.end(); ++it)
	{
		if((*it)->GetUnlocked() && !(*it)->GetPurchased())
			skills.push_back(*it);
	}
	return skills;
}

std::vector<Skill*> SkillManager::GetUnlockedSkills()
{
	std::vector<Skill*> skills;
	for(std::vector<Skill*>::iterator it = skills_.begin(); it != skills_.end(); ++it)
	{
		if((*it)->GetUnlocked())
			skills.push_back(*it);
	}
	return skills;
}

std::vector<Skill*> SkillManager::GetPurchasedSkills()
{
	std::vector<Skill*> skills;
	for(std::vector<Skill*>::iterator it = skills_.begin(); it != skills_.end(); ++it)
	{
		if((*it)->GetPurchased())
			skills.push_back(*it);
	}
	return skills;
}

Skill* SkillManager::GetSkill(std::string _skill)
{
	std::vector<Skill*> skills;
	for(std::vector<Skill*>::iterator it = skills_.begin(); it != skills_.end(); ++it)
	{
		if((*it)->GetName() == _skill )
			return *it;
	}
	return NULL;
}


