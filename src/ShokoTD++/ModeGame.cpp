#include "ModeGame.h"
#include "RenderItem.h"
#include <Widget.h>
#include <GameGridWidget.h>
#include <BlittableRect.h>
#include "Logger.h"
#include "Skills.h"
#include <World.h>
#include <Walker.h>
#include <boost/foreach.hpp>
#include "StandardTextures.h"
#include "RenderItem.h"
#include <Animation.h>
#include <Progression.h>
#include "GridTextureCreator.h"
#include "EnemyTypes.h"
#include "ModeDeckConf.h"
#include "ModeLevelSelect.h"
#include <GameReport.h>
#include <boost/lexical_cast.hpp>

ModeGame::ModeGame(std::string _level_name, std::string _level_file, std::vector<std::string> _skills, Progression* _progression)
{
	level_ = _level_name;
	skills_ = _skills;
	selected_skill_ = Skills::None;
	progression_ = _progression;
	world_ = new World(_level_file);
	ltv_world_state_ = WorldState::OK;
	dialogue_timer_ = 2.5;
}

ModeGame::~ModeGame()
{
	delete world_;
}

IMode* ModeGame::Teardown()
{
	Widget::ClearRoot();
	return IMode::Teardown();
}

void ModeGame::Setup()
{
	assert(skills_.size() <= 8);
	StandardTextures::grid_animation = CreateGridTexture(world_, Vector2i(24, 24));

	Vector2i skill_position(2, 2);
	for(std::vector<std::string>::iterator it = skills_.begin(); it != skills_.end(); it++)
	{
		Widget* skill = new Widget(*it + "_icon.png");
		skill->SetPosition(skill_position);
		skill_position.x += 66;
		skill->SetTag(*it);
		skill->OnClick.connect(boost::bind(&ModeGame::SkillClick, this, _1));
	}

	Widget* back = new Widget("Blank64x64.png");
	back->SetPosition(Vector2i(530, 2));
	back->SetText("Back", TextAlignment::Centre);
	back->OnClick.connect(boost::bind(&ModeGame::QuitClick, this, _1));

	GameGridWidget* click_catcher = new GameGridWidget(Vector2i(26, 17), Vector2i(24, 24));
	click_catcher->SetPosition(Vector2i(8, 69));
	click_catcher->OnGridClick.connect(boost::bind(&ModeGame::GridClick, this, _1, _2));
	click_catcher->OnGridGesture.connect(boost::bind(&ModeGame::GridGesture, this, _1, _2));
	click_catcher->SetOffset(Vector2i(49, 0));

	end_dialogue_ = new Widget("Blank384x384.png");
	
	end_dialogue_->SetPosition(Vector2i(-400, 240 - 192));
	end_dialogue_->SetRejectsFocus(true);
	Widget* continue_widget = new Widget("Blank128x32.png");
	continue_widget->SetText("Next", TextAlignment::Centre);
	continue_widget->SetPosition(Vector2i(246, 344));
	continue_widget->OnClick.connect(boost::bind(&ModeGame::QuitClick, this, _1));
	end_dialogue_->AddChild(continue_widget);

	lives_left_ = new Widget("Blank96x32.png");
	lives_left_->SetPosition(Vector2i(640 - 106, 480 - 42));
	lives_left_->SetRejectsFocus(true);

	lives_left_->SetText(boost::lexical_cast<std::string, int>(world_->GetLives()) + "/" + 
						 boost::lexical_cast<std::string, int>(world_->GetMaxLives()), TextAlignment::Centre);

	
}

ModeAction::Enum ModeGame::Tick(float _dt)
{
	if(ltv_world_state_ == WorldState::OK)
	{
		ltv_world_state_ = world_->Tick(_dt);
		if(ltv_world_state_ == WorldState::FileLoadError)
			end_dialogue_->SetText("Failed to load:\n" + level_, TextAlignment::TopLeft);
		if(ltv_world_state_ == WorldState::Victory)
		{
			end_dialogue_->SetText("You are victorious!", TextAlignment::TopLeft);
			GameReport gr;
			gr.SetAlignment(AlignmentVector(world_->GetGoodKills(), world_->GetEvilKills()));
			progression_->ReportCompletion(level_, gr);
		}
		if(ltv_world_state_ == WorldState::Defeat)
			end_dialogue_->SetText("You are defeated!", TextAlignment::TopLeft);

		lives_left_->SetText(boost::lexical_cast<std::string, int>(world_->GetLives()) + "/" + 
							 boost::lexical_cast<std::string, int>(world_->GetMaxLives()), TextAlignment::Centre);
	} else
	{
		world_->Tick(_dt);
		if(dialogue_timer_ >=0)
			dialogue_timer_ -= _dt;
		if(dialogue_timer_ < 0)
		{
			end_dialogue_->SetModal(true);
			end_dialogue_->SetPosition(Vector2i(320-192, 240 - 192));
		}
	}
	
	BOOST_FOREACH(cooldown_t::value_type &cooldown, cooldowns_)
	{
		cooldown.second -= _dt;
	}

	ModeAction::Enum result = IMode::Tick(_dt);
	Widget::SetFade(fade_);
	return result;
}

ModeType::Enum ModeGame::GetType()
{
	return ModeType::Game;
}


void ModeGame::SkillClick(Widget* _widget)
{
	Skills::Enum skill = Skills::FromString(_widget->GetTag());
	Logger::DiagnosticOut() << "Using skill " << _widget->GetTag() << ", " << skill << ", " << Skills::ToString(skill) <<"\n";
	selected_skill_ = Skills::FromString(_widget->GetTag());
}

void ModeGame::GridClick(Widget* _widget, MouseEventArgs _args)
{
	Logger::DiagnosticOut() << "Clicked grid item " << _args.x << "," << _args.y << "\n";
	switch(selected_skill_)
	{
	case Skills::Arrows:
		if(_args.btns == MouseButton::Right)
			world_->ClearArrow(Vector2i(_args.x, _args.y));
		if(_args.btns == MouseButton::Middle)
			world_->ClearArrows();
		break;
	case Skills::Burn:
		DoBurn(Vector2i(_args.x, _args.y));
		break;
	}
}

void ModeGame::GridGesture(Widget* _widget, GridGestureEventArgs _args)
{
	Direction::Enum direction;
	switch(_args.direction)
	{
	case GestureDirection::West:
		direction = Direction::West;
		break;
	case GestureDirection::East:
		direction = Direction::East;
		break;
	case GestureDirection::South:
		direction = Direction::South;
		break;
	case GestureDirection::North:
		direction = Direction::North;
		break;
	default:
		direction = Direction::Stopped;
	}

	Logger::DiagnosticOut() << "Gesticulated wildly to " << _args.direction << " at " << _args.x << "," << _args.y << "\n";
	if((_args.x < world_->GetSize().x) && (_args.x >= 0) &&
	   (_args.y < world_->GetSize().y) && (_args.y >= 0))
	{
		switch(selected_skill_)
		{
		case Skills::Arrows:
			DoArrows(Vector2i(_args.x, _args.y), direction);
			break;
		case Skills::Burn:
			DoBurn(Vector2i(_args.x, _args.y));
			break;
		}
	}
}


void ModeGame::QuitClick(Widget* _widget)
{
	if(!pend_mode_)
	{
		if(ltv_world_state_ ==WorldState::Victory)
			pend_mode_ = new ModeLevelSelect(progression_);
		else
			pend_mode_ = new ModeDeckConfiguration(level_, progression_);
	}
	
}


void ModeGame::DoArrows(Vector2i _position, Direction::Enum _direction)
{
	if(_direction != Direction::Stopped)
	{
		if(world_->GetArrowsInUse() < progression_->GetSkillsManager().GetSkill("Arrows")->GetSkillLevel("Maximum")->GetLevel() + 1)
		{
			world_->ToggleArrow(_position, _direction, progression_->GetSkillsManager().GetSkill("Arrows")->GetSkillLevel("Durability")->GetLevel() + 1);
		} else
		{
			world_->ClearArrow(_position);
		}
	}
}
void ModeGame::DoBurn(Vector2i _position)
{
	std::vector<Walker*> walkers = world_->GetEnemies();
	for(std::vector<Walker*>::iterator it = walkers.begin(); it != walkers.end(); ++it)
	{
		if(((*it)->GetPosition() - _position).length() < 1)
		{
			float damage = progression_->GetSkillsManager().GetSkill("Burn")->GetSkillLevel("Damage")->GetLevel() * 80;
			float cooldown = 5.0f - progression_->GetSkillsManager().GetSkill("Burn")->GetSkillLevel("Cooldown")->GetLevel() * 0.6f;
			(*it)->TakeEvilDamage(damage);
			cooldowns_["Burn"] = cooldown;
		}
	}
}





std::vector<RenderItem> ModeGame::Draw()
{
	Vector2f grid_size(24, 24);
	const float below = -1;
	const float above = 1;
	vector<RenderItem> draw_list;
	draw_list.reserve(1 + world_->GetEnemies().size() + world_->GetSpawners().size());

	vector<Vector2f> rings = world_->GetProblemPoints();
	BOOST_FOREACH(Vector2f point, rings)
	{
		RenderItem ri;
		ri.position_ = point;
		ri.frame_ = StandardTextures::ring_animation->GetCurrentFrame();
		ri.depth = above;
		draw_list.push_back(ri);
	}

	Walker* p_walker = NULL;
	BOOST_FOREACH(p_walker, world_->GetEnemies())
	{
		RenderItem ri;
		ri.position_ = p_walker->GetPosition();
		ri.frame_ = p_walker->GetEnemyType()->directions[p_walker->GetDirection()]->GetCurrentFrame();
		ri.depth = 0;
		draw_list.push_back(ri);
	}

	BOOST_FOREACH(p_walker, world_->GetDeadEnemies())
	{
		if(p_walker->GetDeathTime() < 1)
		{
			RenderItem ri;
			ri.position_ = p_walker->GetPosition();
			ri.frame_ = p_walker->GetEnemyType()->death_animation->GetFrame(p_walker->GetDeathTime());
			ri.depth = 0;
			draw_list.push_back(ri);
		}
	}

	BOOST_FOREACH(Spawner spawner, world_->GetSpawners())
	{
		RenderItem ri;
		ri.position_ = spawner.Position;
		ri.frame_ = StandardTextures::spawner_animation->GetCurrentFrame();
		ri.depth = below;
		draw_list.push_back(ri);
	}


	for(int x = 0; x < world_->GetSize().x; x++)
	{
		for(int y = 0; y < world_->GetSize().y; y++)
		{
			SquareType::Enum square_type = world_->GetSquareType(Vector2i(x, y));
			switch(square_type)
			{
			case SquareType::Rocket:
				{
					RenderItem ri;
					ri.position_ = Vector2f((float)x, (float)y);
					ri.frame_ = StandardTextures::rocket_normal_animation->GetCurrentFrame();
					ri.depth = below;
					draw_list.push_back(ri);
				}
				break;
			case SquareType::Hole:
				{
					RenderItem ri;
					ri.position_ = Vector2f((float)x, (float)y);
					ri.frame_ = StandardTextures::hole_animation->GetCurrentFrame();
					ri.depth = below;
					draw_list.push_back(ri);
				}
				break;
			case SquareType::Cross:
				{
					RenderItem ri;
					ri.position_ = Vector2f((float)x, (float)y);
					ri.frame_ = StandardTextures::cross_animation->GetCurrentFrame();
					ri.depth = below;
					draw_list.push_back(ri);
				}
				break;
			case SquareType::NorthArrow:
			case SquareType::SouthArrow:
			case SquareType::EastArrow:
			case SquareType::WestArrow:
				{
					int square_count = world_->GetSquareCount(Vector2i(x, y));
					RenderItem ri;
					ri.position_ = Vector2f((float)x, (float)y);
					if(square_count > 1)
						ri.frame_ = StandardTextures::arrows[SquareType::GetDirection(square_type)]->GetCurrentFrame();
					else
						ri.frame_ = StandardTextures::half_arrows[SquareType::GetDirection(square_type)]->GetCurrentFrame();
					ri.depth = below;
					draw_list.push_back(ri);
				}
				break;
			}
		}
	}

	BOOST_FOREACH(RenderItem& render_item, draw_list)
	{
		render_item.position_ *= grid_size;
		render_item.position_ += Vector2i(2, 2);
		render_item.depth *= grid_size.y;
	}

	{
		RenderItem ri;
		ri.position_ = Vector2f(0, 0);
		ri.frame_ = StandardTextures::grid_animation->GetCurrentFrame();
		ri.depth = below * grid_size.y;	
		draw_list.push_back(ri);
	}

	return draw_list;
}
