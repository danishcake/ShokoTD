#include "ModeDeckConf.h"
#include "RenderItem.h"
#include <Widget.h>
#include <ItemBrowserWidget.h>
#include <BlittableRect.h>
#include "Logger.h"
#include "ModeGame.h"
#include "ModeLevelSelect.h"
#include "Progression.h"
#include "Skill.h"
#include <boost/lexical_cast.hpp>


ModeDeckConfiguration::ModeDeckConfiguration(std::string _next_level, Progression* _progression)
{
	progression_ = _progression;
	next_level_ = _next_level;
	skill_needs_update_ = false;
}

IMode* ModeDeckConfiguration::Teardown()
{
	Widget::ClearRoot();
	return IMode::Teardown();
}

void ModeDeckConfiguration::Setup()
{
	std::vector<Skill*> purchased_skills_ = progression_->GetSkillsManager().GetPurchasedSkills();
	std::vector<Skill*> unlocked_skills_ = progression_->GetSkillsManager().GetUnlockedSkills();

	std::vector<std::string> items;
	for(std::vector<Skill*>::iterator it = purchased_skills_ .begin(); it != purchased_skills_ .end(); ++it)
	{
		items.push_back((*it)->GetName());
	}
	for(std::vector<Skill*>::iterator it = unlocked_skills_ .begin(); it != unlocked_skills_ .end(); ++it)
	{
		if(std::find(items.begin(), items.end(), (*it)->GetName()) == items.end())
			items.push_back((*it)->GetName());
	}
	known_skills_ = new ItemBrowserWidget(items, Vector2i(2, 7), Vector2i(64, 64));
	known_skills_->OnItemRender.connect(boost::bind(&ModeDeckConfiguration::ItemRender, this, _1, _2, _3));
	known_skills_->PerformItemLayout();
	known_skills_->OnItemDragStart.connect(boost::bind(&ModeDeckConfiguration::ItemDragStart, this, _1, _2));
	known_skills_->OnItemClick.connect(boost::bind(&ModeDeckConfiguration::ItemClick, this, _1, _2));

	std::vector<std::string> none;
	selected_skills_ = new ItemBrowserWidget(none, Vector2i(7, 1), Vector2i(64, 64));
	selected_skills_->SetPosition(Vector2i(128 + 20, 10));
	selected_skills_->SetAllowScroll(false);
	selected_skills_->OnItemRender.connect(boost::bind(&ModeDeckConfiguration::ItemRender, this, _1, _2, _3));
	selected_skills_->PerformItemLayout();
	
	selected_skills_->OnDragEnter.connect(boost::bind(&ModeDeckConfiguration::ItemDragEnter, this, _1, _2));
	selected_skills_->OnDragLand.connect(boost::bind(&ModeDeckConfiguration::ItemDragLand, this, _1, _2));
	
	selected_skills_->OnItemDragEnter.connect(boost::bind(&ModeDeckConfiguration::ItemDragEnter, this, _1, _2));
	selected_skills_->OnItemDragLand.connect(boost::bind(&ModeDeckConfiguration::ItemDragLand, this, _1, _2));
	selected_skills_->OnItemClick.connect(boost::bind(&ModeDeckConfiguration::ItemClick, this, _1, _2));

	Widget* next_level = new Widget("Blank96x32.png");
	next_level->SetPosition(Vector2i(534, 438));
	next_level->SetText("Done", TextAlignment::Centre);
	next_level->OnClick.connect(boost::bind(&ModeDeckConfiguration::Accept, this, _1));

	upgrades_pane_ = new Widget("Blank384x384.png");
	upgrades_pane_->SetPosition(Vector2i(148, 84));
	upgrades_pane_->SetRejectsFocus(true);
	upgrades_pane_->SetText("Drag skills to bar\nThen upgrade them here!", TextAlignment::Centre);

	Widget* go_back = new Widget("Blank96x32.png");
	go_back->SetPosition(Vector2i(534, 396));
	go_back->SetText("Back", TextAlignment::Centre);
	go_back->OnClick.connect(boost::bind(&ModeDeckConfiguration::GoBack, this, _1));

	good_points_ = new Widget("Blank96x64.png");
	good_points_->SetPosition(Vector2i(534, 84));
	good_points_->SetRejectsFocus(true);

	evil_points_ = new Widget("Blank96x64.png");
	evil_points_->SetPosition(Vector2i(534, 158));
	evil_points_->SetRejectsFocus(true);
}

ModeAction::Enum ModeDeckConfiguration::Tick(float _dt)
{
	UpdateUpgradesPane();
	good_points_->SetText("Good\n" + boost::lexical_cast<std::string, unsigned int>(progression_->GetGoodPoints()), TextAlignment::Centre);
	evil_points_->SetText("Evil\n" + boost::lexical_cast<std::string, unsigned int>(progression_->GetEvilPoints()), TextAlignment::Centre);

	ModeAction::Enum result = IMode::Tick(_dt);
	Widget::SetFade(fade_);
	return result;
}

void ModeDeckConfiguration::UpdateUpgradesPane()
{
	if(skill_needs_update_)
	{
		skill_needs_update_ = false;
		Skill* skill = progression_->GetSkillsManager().GetSkill(last_skill_selected_);
		if(skill)
		{
			if(skill->GetPurchased())
			{
				std::vector<SkillLevel> skill_levels = skill->GetUpgrades();
				upgrades_pane_->ClearChildren();
				switch(skill->GetAlignment())
				{
				case SkillAlignment::Good:
					upgrades_pane_->SetText("Upgrades:\nAlignment: Good", TextAlignment::TopLeft);
					break;
				case SkillAlignment::Neutral:
					upgrades_pane_->SetText("Upgrades:\nAlignment: Neutral", TextAlignment::TopLeft);
					break;
				case SkillAlignment::Evil:
					upgrades_pane_->SetText("Upgrades:\nAlignment: Evil", TextAlignment::TopLeft);
					break;
				}
				
				int upgrade_id = 0;
				for(std::vector<SkillLevel>::iterator it = skill_levels.begin(); it != skill_levels.end(); ++it)
				{
					Widget* upgrade_name = new Widget("Blank192x32.png");
					upgrade_name->SetPosition(Vector2i(10, 74) + Vector2i(0, 42) * upgrade_id);
					upgrade_name->SetText(it->GetName(), TextAlignment::Centre);
					upgrade_name->SetRejectsFocus(true);
					upgrades_pane_->AddChild(upgrade_name);
					Widget* current_rank = new Widget("Blank32x32.png");
					current_rank->SetText(boost::lexical_cast<std::string, int>(it->GetLevel()), TextAlignment::Centre);
					current_rank->SetPosition(Vector2i(212, 74) + Vector2i(0, 42) * upgrade_id);
					current_rank->SetRejectsFocus(true);
					upgrades_pane_->AddChild(current_rank);
					if(it->GetLevel() < it->GetMaxLevel())
					{
						Widget* upgrade_cost = new Widget("Blank64x32.png");
						upgrade_cost->SetPosition((Vector2i(254, 74) + Vector2i(0, 42) * upgrade_id));
						upgrade_cost->SetText(boost::lexical_cast<std::string, unsigned int>(it->GetUpgradeCost()), TextAlignment::Centre);
						upgrade_cost->SetRejectsFocus(true);
						upgrades_pane_->AddChild(upgrade_cost);

						Widget* upgrade_plus_one = new Widget("Blank32x32.png");
						upgrade_plus_one->SetPosition(Vector2i(326, 74) + Vector2i(0, 42) * upgrade_id);
						upgrade_plus_one->SetText("+", TextAlignment::Centre);
						upgrade_plus_one->SetTag(it->GetName());
						upgrades_pane_->AddChild(upgrade_plus_one);
						upgrade_plus_one->OnClick.connect(boost::bind(&ModeDeckConfiguration::UpgradeSkillClick, this, _1));
					}
					upgrade_id++;
				}
			} else
			{
				upgrades_pane_->ClearChildren();
				std::string skill_text;
				switch(skill->GetAlignment())
				{
				case SkillAlignment::Good:
					skill_text = "Skill locked\nAlignment: Good\n";
					break;
				case SkillAlignment::Neutral:
					skill_text = "Skill locked\nAlignment: Neutral\n";
					break;
				case SkillAlignment::Evil:
					skill_text = "Skill locked\nAlignment: Evil\n";
					break;
				}
				upgrades_pane_->SetText(skill_text + "Cost: " + boost::lexical_cast<std::string, unsigned int>(skill->GetCost()), TextAlignment::TopLeft);
				Widget* buy_skill = new Widget("Blank96x32.png");
				buy_skill->SetPosition(Vector2i(10, 106));
				buy_skill->SetText("Buy", TextAlignment::Centre);
				buy_skill->OnClick.connect(boost::bind(&ModeDeckConfiguration::BuySkillClick, this, _1));
				upgrades_pane_->AddChild(buy_skill);
			}
		}
	}
}

ModeType::Enum ModeDeckConfiguration::GetType()
{
	return ModeType::DeckConfiguration;
}

std::vector<RenderItem> ModeDeckConfiguration::Draw()
{
	return std::vector<RenderItem>();
}

void ModeDeckConfiguration::BuySkillClick(Widget* _widget)
{
	Skill* skill = progression_->GetSkillsManager().GetSkill(last_skill_selected_);
	switch(skill->GetAlignment())
	{
		case SkillAlignment::Good:
			if(progression_->GetGoodPoints() >= skill->GetCost())
			{
				progression_->SpendPoints(skill->GetCost(), 0);
				skill->SetPurchased(true);
				known_skills_->PerformItemLayout();
				skill_needs_update_ = true;
			}
			break;
		case SkillAlignment::Neutral:
			if(progression_->GetGoodPoints() >= skill->GetCost() &&
			   progression_->GetEvilPoints() >= skill->GetCost())
			{
				progression_->SpendPoints(skill->GetCost(), skill->GetCost());
				skill->SetPurchased(true);
				known_skills_->PerformItemLayout();
				skill_needs_update_ = true;
			}
			break;
		case SkillAlignment::Evil:
			if(progression_->GetEvilPoints() >= skill->GetCost())
			{
				progression_->SpendPoints(0, skill->GetCost());
				skill->SetPurchased(true);
				known_skills_->PerformItemLayout();
				skill_needs_update_ = true;
			}
			break;
	}
}

void ModeDeckConfiguration::UpgradeSkillClick(Widget* _widget)
{
	Skill* skill = progression_->GetSkillsManager().GetSkill(last_skill_selected_);
	std::string skill_name = _widget->GetTag();
	SkillLevel* skill_level = skill->GetSkillLevel(skill_name);

	switch(skill->GetAlignment())
	{
		case SkillAlignment::Good:
			if(progression_->GetGoodPoints() >= skill_level->GetUpgradeCost())
			{
				progression_->SpendPoints(skill_level->GetUpgradeCost(), 0);
				skill_level->Upgrade();
				skill_needs_update_ = true;
			}
			break;
		case SkillAlignment::Neutral:
			if(progression_->GetGoodPoints() >= skill_level->GetUpgradeCost() &&
			   progression_->GetEvilPoints() >= skill_level->GetUpgradeCost())
			{
				progression_->SpendPoints(skill_level->GetUpgradeCost(), skill_level->GetUpgradeCost());
				skill_level->Upgrade();
				skill_needs_update_ = true;
			}
			break;
		case SkillAlignment::Evil:
			if(progression_->GetEvilPoints() >= skill_level->GetUpgradeCost())
			{
				progression_->SpendPoints(0, skill_level->GetUpgradeCost());
				skill_level->Upgrade();
				skill_needs_update_ = true;
			}
			break;
	}
}

void ModeDeckConfiguration::ItemRender(Widget* _widget, BlittableRect** _blittable, std::string _name)
{
	*_blittable = new BlittableRect(_name + "_icon.png");

	if(!progression_->GetSkillsManager().SkillPurchased(_name))
	{
		BlittableRect lock("Padlock.png");
		lock.Blit(Vector2i(0, 0), *_blittable);
	}
}

void ModeDeckConfiguration::ItemDragLand(Widget* _widget, DragEventArgs* _drag_args)
{
	std::string tag = ((Widget*)_drag_args->data)->GetTag();
	Logger::DiagnosticOut() << "Drag land of type " << tag << "\n";
	std::vector<std::string> items = selected_skills_->GetItems();
	if(std::find(items.begin(), items.end(), tag) == items.end())
	{
		items.push_back(tag);
		selected_skills_->SetItems(items);
	}
}

void ModeDeckConfiguration::ItemDragStart(Widget* _widget, DragEventArgs* _drag_args)
{
	static int c = 0;
	c++;
	Logger::DiagnosticOut() << "Drag start:" << c << "\n";
	_drag_args->drag_type = 1;
	_drag_args->data = _widget;
}

void ModeDeckConfiguration::ItemDragEnter(Widget* _widget, DragEventArgs* _drag_args)
{
	_drag_args->drag_accepted = _drag_args->drag_type == 1;
}

void ModeDeckConfiguration::Accept(Widget* _widget)
{
	pend_mode_ = new ModeGame(next_level_, progression_->GetLevelFilename(next_level_), selected_skills_->GetItems(), progression_);
}

void ModeDeckConfiguration::GoBack(Widget* _widget)
{
	pend_mode_ = new ModeLevelSelect(progression_);
}

void ModeDeckConfiguration::ItemClick(Widget* _widget, std::string _item)
{
	Skill* skill = progression_->GetSkillsManager().GetSkill(_item);
	if(skill)
	{
		skill_needs_update_ = true;
		last_skill_selected_ = _item;
	}
}