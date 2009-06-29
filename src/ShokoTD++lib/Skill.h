#pragma once
#include <string>
#include <vector>
#include "SkillLevel.h"


namespace SkillAlignment
{
	enum Enum
	{
		Good, Evil, Neutral
	};
}

class Skill
{
protected:
	std::string skill_name_;
	bool unlocked_;
	bool purchased_;
	unsigned int initial_cost_;

	std::vector<SkillLevel> upgrades_;
	SkillAlignment::Enum skill_alignment_;
public:
	Skill(void);
	~Skill(void);

	bool GetUnlocked(){return unlocked_;}
	void SetUnlocked(bool _unlocked){unlocked_ = _unlocked;}

	bool GetPurchased(){return purchased_;}
	void SetPurchased(bool _purchased){purchased_ = _purchased;}

	std::vector<SkillLevel> GetUpgrades(){return upgrades_;}
	void AddUpgrade(SkillLevel _upgrade){upgrades_.push_back(_upgrade);}
	SkillLevel* GetSkillLevel(std::string _name);

	unsigned int GetCost(){return initial_cost_;}
	void SetCost(unsigned int _cost){initial_cost_ = _cost;}

	SkillAlignment::Enum GetAlignment(){return skill_alignment_;}
	void SetAlignment(SkillAlignment::Enum _alignment){skill_alignment_ = _alignment;}

	std::string GetName(){return skill_name_;}
	void SetName(std::string _skill_name){skill_name_ = _skill_name;}
};

