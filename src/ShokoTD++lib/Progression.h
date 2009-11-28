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

struct CampaignDescription
{
	std::string name;
	std::string filename;
	std::string description;
	std::string icon_filename;
	int unlockable_count;
	bool save_exists;
};


class Progression
{
private:
	std::string campaign_;
	std::string savefile_;
	CampaignDescription description_;
	
	std::vector<ProgressLevel*> levels_; //Map might have made more sense
	
	SkillManager skill_manager_;
	std::vector<std::string> skill_deck_;
	
	ProgressionState::Enum state_;

	bool LoadCampaign(TiXmlDocument* _doc);
	bool LoadUnlockables(TiXmlElement* _first, ProgressLevel* _progress_level);
	bool LoadSavefile();

	unsigned int good_points_;
	unsigned int evil_points_;
	static bool cheat_;
	bool disable_save_;

public:
	Progression(std::string _campaign, std::string _savefile, bool _resume);
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
	std::vector<std::string>& GetSkillDeck(){return skill_deck_;}

	/* Score */
	unsigned int GetGoodPoints(){return good_points_;}
	unsigned int GetEvilPoints(){return evil_points_;}
	void SpendPoints(unsigned int _good, unsigned int _evil){good_points_ -= _good; evil_points_ -= _evil;}


	/* Other */
	bool GetError(){return state_ != ProgressionState::OK;}

	void DisableSave(){disable_save_ = true;}
	CampaignDescription GetDescription(){return description_;}

	static void SetCheatMode(){cheat_ = true;}

	static std::string GetCampaignIcon(std::string _campaign);
};