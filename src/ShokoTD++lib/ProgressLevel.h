#pragma once
#include <vector>
#include <string>
class GameReport;

struct Unlockable
{
	struct 
	{
		bool good;
		bool evil;
		bool flawless;
	} condition;
	struct
	{
		std::vector<std::string> levels;
		std::vector<std::string> skills;
	} reward;
};

class ProgressLevel
{
private:
	std::string name_;
	std::string filename_;

	bool locked_;
	bool beaten_;
	std::vector<Unlockable> unlockables_;
public:
	ProgressLevel(std::string _name, std::string _filename);

	/* Getters and Setters */
	void SetName(std::string _name){name_ = _name;}
	std::string GetName(){return name_;}
	void SetFilename(std::string _filename){filename_= _filename;}
	std::string GetFilename(){return filename_;}

	void SetLocked(bool _locked){locked_ = _locked;}
	bool GetLocked(){return locked_;}

	void SetBeaten(bool _beaten){beaten_ = _beaten;}
	bool GetBeaten(){return beaten_;}

	void AddUnlock(bool _good, bool _evil, bool _flawless, std::vector<std::string> _levels, std::vector<std::string> _skills);
	void GetRewards(GameReport _gr, std::vector<std::string> &_levels, std::vector<std::string>& _skills);
	void GetUnlockables(std::vector<std::string> &_levels, std::vector<std::string>& _skills);


};
