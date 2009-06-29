#pragma once
class Skill;
#include <vector>
#include <string>

class SkillManager
{
private:
	bool error_;
	std::vector<Skill*> skills_;
public:
	SkillManager(void);
	~SkillManager(void);

	bool GetError(){return error_;}

	std::vector<Skill*> GetAllSkills(){return skills_;}

	void Unlock(std::string _skill);
	void Purchase(std::string _skill);
	bool SkillExists(std::string _skill);
	bool SkillAvailable(std::string _skill);
	bool SkillPurchased(std::string _skill);

	std::vector<Skill*> GetUnlockedSkills();
	std::vector<Skill*> GetPurchasedSkills();
	std::vector<Skill*> GetAvailableSkills(); //Means unlocked but not purchased

	Skill* GetSkill(std::string _skill);

};
