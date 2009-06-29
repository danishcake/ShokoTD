#pragma once
#include <string>
#include <vector>


class SkillLevel
{
private:
	std::string level_name_;
	unsigned int max_level_;
	unsigned int level_;
	std::vector<int> rank_costs_;
public:
	SkillLevel(std::string _level_name);
	
	unsigned int GetLevel(){return level_;}
	void SetLevel(unsigned int _level){level_ = _level;}
	unsigned int GetMaxLevel(){return max_level_;}
	void SetMaxLevel(unsigned int _max_level){max_level_ = _max_level;}
	std::string GetName(){return level_name_;}
	
	void AddRank(unsigned int _cost);

	unsigned int GetUpgradeCost();
	void Upgrade();
};

