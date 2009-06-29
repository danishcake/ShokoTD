#pragma once
#include <vector>
#include <string>
#include "SkillManager.h"
#include "Skill.h"
class Skill;
class ProgressLevel;
class GameReport;
class TiXmlDocument;
class TiXmlElement;


namespace ProgressionState
{
	enum Enum
	{
		OK, LoadError
	};
}

class Progression
{
private:
	std::string campaign_;
	
	std::vector<ProgressLevel*> levels_; //Map might have made more sense
	
	SkillManager skill_manager_;;
	
	ProgressionState::Enum state_;

	bool LoadCampaign(TiXmlDocument* _doc);
	bool Progression::LoadUnlockables(TiXmlElement* _first, ProgressLevel* _progress_level);
public:
	Progression(std::string _campaign, std::string _savefile);
	~Progression();

	/* Levels */
	std::vector<ProgressLevel*> GetUnlocked();
	std::vector<ProgressLevel*> GetLocked();
	bool Has(std::string _level);
	bool IsUnlocked(std::string _level);
	bool IsBeaten(std::string _level);
	void Unlock(std::string _level);
	void ReportCompletion(std::string _level, GameReport _gr);
	void GetRewards(std::string _level, std::vector<std::string>& _unlocked_skills, 
										std::vector<std::string>& _locked_skills,
										std::vector<std::string>& _unlocked_levels,
										std::vector<std::string>& _locked_levels);
	std::string GetLevelFilename(std::string _name);

	/* Skills */
	SkillManager& GetSkillsManager(){return skill_manager_;}

	/* Other */
	bool GetError(){return state_ != ProgressionState::OK;}
};