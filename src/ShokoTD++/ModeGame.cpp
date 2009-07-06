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

bool ModeGame::show_health_ = true;

ModeGame::ModeGame(std::string _level_name, std::string _level_file, Progression* _progression)
{
	level_ = _level_name;
	skills_ = _progression->GetSkillDeck();
	selected_skill_ = Skills::None;
	progression_ = _progression;
	world_ = new World(_level_file);
	ltv_world_state_ = WorldState::OK;
	dialogue_timer_ = 2.5;
}

ModeGame::~ModeGame()
{
	delete world_;
	for(vector<BlittableRect*>::iterator it = cooldown_overlays_.begin(); it != cooldown_overlays_.end(); ++it)
	{
		delete *it;
	}
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

	cooldown_overlays_.push_back(new BlittableRect("Cooldown0.png"));
	cooldown_overlays_.push_back(new BlittableRect("Cooldown1.png"));
	cooldown_overlays_.push_back(new BlittableRect("Cooldown2.png"));
	cooldown_overlays_.push_back(new BlittableRect("Cooldown3.png"));
	cooldown_overlays_.push_back(new BlittableRect("Cooldown4.png"));
	cooldown_overlays_.push_back(new BlittableRect("Cooldown5.png"));
	cooldown_overlays_.push_back(new BlittableRect("Cooldown6.png"));
	cooldown_overlays_.push_back(new BlittableRect("Cooldown7.png"));
	cooldown_overlays_.push_back(new BlittableRect("Cooldown8.png"));
	cooldown_overlays_.push_back(new BlittableRect("Cooldown9.png"));
		

	Vector2i skill_position(2, 2);
	int skill_id = 1;
	for(std::vector<std::string>::iterator it = skills_.begin(); it != skills_.end(); it++)
	{
		Widget* skill = new Widget(*it + "_icon.png");
		
		if(skill_id != skills_.size())
			skill->GetBackRect()->BlitText(boost::lexical_cast<std::string, int>(skill_id), TextAlignment::BottomRight);
		skill->SetPosition(skill_position);
		skill_position.x += 66;
		skill->SetTag(*it);
		skill->OnClick.connect(boost::bind(&ModeGame::SkillClick, this, _1));
		skill->OnDraw.connect(boost::bind(&ModeGame::SkillRedraw, this, _1, _2));
		skill_id++;
		cooldown_widgets_[*it] = skill;
	}

	Widget* back = new Widget("Blank64x64.png");
	back->SetPosition(Vector2i(530, 2));
	back->SetText("Back", TextAlignment::Centre);
	back->OnClick.connect(boost::bind(&ModeGame::QuitClick, this, _1));

	GameGridWidget* click_catcher = new GameGridWidget(Vector2i(26, 17), Vector2i(24, 24));
	click_catcher->SetPosition(Vector2i(8, 69));
	click_catcher->OnGridClick.connect(boost::bind(&ModeGame::GridClick, this, _1, _2));
	click_catcher->OnGridGesture.connect(boost::bind(&ModeGame::GridGesture, this, _1, _2));
	click_catcher->SetOffset(Vector2i(0, 0));
	click_catcher->OnMouseMove.connect(boost::bind(&ModeGame::GridMove, this, _1, _2));

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

	keypress_connection_ = Widget::OnGlobalKeyUp.connect(boost::bind(&ModeGame::Keypress, this, _1, _2));
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
			GameReport gr;
			gr.SetAlignment(AlignmentVector(world_->GetGoodKills(), world_->GetEvilKills()));
			gr.SetFlawless(world_->GetLives() == world_->GetMaxLives());
			std::string end_text = "You are victorious!\n\nYou saved:"  + boost::lexical_cast<std::string, int>(world_->GetGoodKills()) +
														"\nYou doomed:" + boost::lexical_cast<std::string, int>(world_->GetEvilKills()) +
														"\nSlippery slope effect:\nGood: +" + boost::lexical_cast<std::string, int>(gr.GetAlignment().GetGoodScore()) +
														"\nEvil: +" + boost::lexical_cast<std::string, int>(gr.GetAlignment().GetEvilScore());

			end_dialogue_->SetText(end_text, TextAlignment::TopLeft);
			

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

	BOOST_FOREACH(widget_cooldown_t::value_type &widget_cooldown, cooldown_widgets_)
	{
		widget_cooldown.second->Invalidate();
	}

	BOOST_FOREACH(Decoration& decoration, decorations_)
	{
		decoration.time_to_live -= _dt;
	}

	decorations_.erase(std::remove_if(decorations_.begin(), decorations_.end(), boost::bind(&Decoration::IsDead, _1)), decorations_.end());


	ModeAction::Enum result = IMode::Tick(_dt);
	Widget::SetFade(fade_);
	return result;
}

ModeType::Enum ModeGame::GetType()
{
	return ModeType::Game;
}

void ModeGame::SkillRedraw(Widget* _widget, BlittableRect* _blittable)
{
	if(cooldowns_.find(_widget->GetTag()) != cooldowns_.end())
	{
		float cooldown = GetCooldown(_widget->GetTag());
		float frac = 1.0f - (cooldowns_[_widget->GetTag()] / cooldown);
		frac = frac < 0 ? frac = 0 : frac > 1.0f ? 1.0f : frac;
		int index = frac * 10;
		if(index < 10)
			cooldown_overlays_[index]->Blit(Vector2i(0, 0), _blittable);
	}
}

void ModeGame::SkillClick(Widget* _widget)
{
	Skills::Enum skill = Skills::FromString(_widget->GetTag());
	Logger::DiagnosticOut() << "Using skill " << _widget->GetTag() << ", " << skill << ", " << Skills::ToString(skill) <<"\n";
	selected_skill_ = Skills::FromString(_widget->GetTag());

	switch(selected_skill_)
	{
	case Skills::PermaSlow:
		DoPermaSlow();
		break;
	case Skills::SpawnPause:
		DoSpawnPause();
		break;
	}

}

void ModeGame::Keypress(Widget* _widget, KeyPressEventArgs _args)
{
	if((last_grid_position_.x < world_->GetSize().x) && (last_grid_position_.x >= 0) &&
	   (last_grid_position_.y < world_->GetSize().y) && (last_grid_position_.y >= 0))
	{
		if(_args.key_code >= 49 && _args.key_code <= 59)
		{
			int skill_index = _args.key_code - 49;
			if(skill_index < skills_.size())
			{
				Skills::Enum skill = Skills::FromString(skills_[skill_index]);
				switch(skill)
				{
				case Skills::Burn:
					DoBurn(last_grid_position_);
					break;
				case Skills::Craze:
					DoCraze(last_grid_position_);
					break;
				case Skills::PermaSlow:
					DoPermaSlow();
					break;
				case Skills::SpawnPause:
					DoSpawnPause();
					break;
				case Skills::Slow:
					DoSlow(last_grid_position_);
					break;

				}
			}
		} else if(_args.key_code == 119) //W
		{ 
			DoArrows(last_grid_position_, Direction::North);
		} else if(_args.key_code == 97)  //A
		{
			DoArrows(last_grid_position_, Direction::West);
		} else if(_args.key_code == 115) //S
		{
			DoArrows(last_grid_position_, Direction::South);
		} else if(_args.key_code == 100) //D
		{
			DoArrows(last_grid_position_, Direction::East);
		}
	}
	if(_args.key_code == 104) //H
	{
		show_health_ = !show_health_;
	}
}

void ModeGame::GridMove(Widget* _widget, MouseEventArgs _args)
{
	//Logger::DiagnosticOut() << "Mouse moved to " << _args.x << "," << _args.y << "\n";
	last_grid_position_ = Vector2i(_args.x / 24, _args.y / 24);
	//Logger::DiagnosticOut() << "Mouse moved to " << last_grid_position_ << "\n";
}

void ModeGame::GridClick(Widget* _widget, MouseEventArgs _args)
{
	Logger::DiagnosticOut() << "Clicked grid item " << _args.x << "," << _args.y << "\n";
	if((_args.x < world_->GetSize().x) && (_args.x >= 0) &&
	   (_args.y < world_->GetSize().y) && (_args.y >= 0))
	{
		if(_args.btns == MouseButton::Right)
			world_->ClearArrow(Vector2i(_args.x, _args.y));
		if(_args.btns == MouseButton::Middle)
			world_->ClearArrows();
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
		DoArrows(Vector2i(_args.x, _args.y), direction);
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

float ModeGame::GetCooldown(std::string _skill)
{
	float cooldown = 1;
	Skills::Enum skill = Skills::FromString(_skill);
	if(skill != Skills::None || skill != Skills::Arrows)
	{
		int cooldown_level = progression_->GetSkillsManager().GetSkill(_skill)->GetSkillLevel("Cooldown")->GetLevel();

		switch(skill)
		{
		case Skills::Burn:
			cooldown = 4.0f - cooldown_level * 0.6f;
			break;
		case Skills::Craze:
			cooldown = 4.0f - cooldown_level * 0.6f;
			break;
		case Skills::Slow:
			cooldown = 4.0f - cooldown_level * 0.6f;
			break;
		case Skills::PermaSlow:
			cooldown = 16.5 - cooldown_level * 1.5f;
			break;
		case Skills::SpawnPause:
			cooldown = 16.5 - cooldown_level * 1.5f;
			break;
		}
	}
	return cooldown;
}

bool ModeGame::CooldownOK(std::string _skillname)
{
	if(cooldowns_.find(_skillname) != cooldowns_.end())
	{
		return cooldowns_[_skillname] <= 0;
	}
	return true;
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
	if(!CooldownOK("Burn"))
		return;
	
	float damage = progression_->GetSkillsManager().GetSkill("Burn")->GetSkillLevel("Damage")->GetLevel() * 80;
	float cooldown = GetCooldown("Burn");

	std::vector<Walker*> walkers = world_->GetEnemies();
	for(std::vector<Walker*>::iterator it = walkers.begin(); it != walkers.end(); ++it)
	{
		if(((*it)->GetPosition() - _position).length() < 1)
		{
			(*it)->TakeEvilDamage(damage);
		}
	}
	Decoration d;
	d.animation = StandardTextures::burning_animation;
	d.position = _position;
	d.time_to_live = 0.5;
	decorations_.push_back(d);
	
	cooldowns_["Burn"] = cooldown;
}

void ModeGame::DoSlow(Vector2i _position)
{
	if(!CooldownOK("Slow"))
		return;

	float duration = progression_->GetSkillsManager().GetSkill("Slow")->GetSkillLevel("Duration")->GetLevel() * 1.0f + 2.0f;
	float cooldown = GetCooldown("Slow");

	std::vector<Walker*> walkers = world_->GetEnemies();
	for(std::vector<Walker*>::iterator it = walkers.begin(); it != walkers.end(); ++it)
	{
		if(((*it)->GetPosition() - _position).length() < 2)
		{
			(*it)->SlowDown(duration, 0.3f);
		}
	}
	Decoration d;
	d.animation = StandardTextures::slow_animation;
	d.position = _position;
	d.time_to_live = 1;
	decorations_.push_back(d);
	
	cooldowns_["Slow"] = cooldown;
}

void ModeGame::DoCraze(Vector2i _position)
{
	if(!CooldownOK("Craze"))
		return;

	float duration = progression_->GetSkillsManager().GetSkill("Craze")->GetSkillLevel("Duration")->GetLevel() * 1.0f + 2.0f;
	float cooldown = GetCooldown("Craze");

	std::vector<Walker*> walkers = world_->GetEnemies();
	for(std::vector<Walker*>::iterator it = walkers.begin(); it != walkers.end(); ++it)
	{
		if(((*it)->GetPosition() - _position).length() < 2)
		{
			(*it)->SetCrazed(duration);
			(*it)->SlowDown(duration, 0.7f);
		}
	}
	Decoration d;
	d.animation = StandardTextures::craze_animation;
	d.position = _position;
	d.time_to_live = 2.5f;
	decorations_.push_back(d);
	
	cooldowns_["Craze"] = cooldown;
}

void ModeGame::DoSpawnPause()
{
	if(!CooldownOK("SpawnPause"))
		return;

	float cooldown = GetCooldown("SpawnPause");
	float duration = 4 + progression_->GetSkillsManager().GetSkill("SpawnPause")->GetSkillLevel("Duration")->GetLevel() * 1.0f;

	world_->SpawnPause(duration);
	

	cooldowns_["SpawnPause"] = cooldown;
}

void ModeGame::DoPermaSlow()
{
	if(!CooldownOK("PermaSlow"))
		return;

	float cooldown = GetCooldown("PermaSlow");

	std::vector<Walker*> walkers = world_->GetEnemies();
	for(std::vector<Walker*>::iterator it = walkers.begin(); it != walkers.end(); ++it)
	{
		(*it)->SlowDown(10000.0f, 0.25f); //Approximately forver ;)
	}

	cooldowns_["PermaSlow"] = cooldown;
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

		if(show_health_)
		{
			RenderItem ri2;
			ri2.position_ = p_walker->GetPosition() + Vector2f(0, 1.05f);
			int frame = (1.0f - (p_walker->GetHealth() / p_walker->GetMaxHealth())) * 9;
			ri2.frame_ = StandardTextures::healthbar_animation->GetFrameByIndex(frame);
			ri2.depth = above * 2;
			draw_list.push_back(ri2);
		}
	}

	BOOST_FOREACH(p_walker, world_->GetDeadEnemies())
	{
		if(p_walker->GetDeathTime() < 1)
		{
			RenderItem ri;
			ri.position_ = p_walker->GetPosition();
			ri.frame_ = p_walker->GetEnemyType()->death_animation->GetFrame(p_walker->GetDeathTime());
			ri.depth = above * 2;
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

	BOOST_FOREACH(Decoration& decoration, decorations_)
	{
		RenderItem ri;
		ri.position_ = decoration.position;
		ri.frame_ = decoration.animation->GetCurrentFrame();
		ri.depth = above;
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
