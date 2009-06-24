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


ModeGame::ModeGame(std::string _level_name, std::vector<std::string> _skills, Progression* _progression)
{
	level_ = _level_name;
	skills_ = _skills;
	progression_ = _progression;
	world_ = new World(level_);
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
	back->SetText("Menu", TextAlignment::Centre);

	GameGridWidget* click_catcher = new GameGridWidget(Vector2i(26, 17), Vector2i(24, 24));
	click_catcher->SetPosition(Vector2i(8, 69));
	click_catcher->OnGridClick.connect(boost::bind(&ModeGame::GridClick, this, _1, _2));
	//click_catcher->OnGridGesture.connect(boost::bind(&ModeGame::GridGesture, this, _1, _2));
	click_catcher->SetOffset(Vector2i(49, 0));
	
}

ModeAction::Enum ModeGame::Tick(float _dt)
{
	world_->Tick(_dt);
	ModeAction::Enum result = IMode::Tick(_dt);
	Widget::SetFade(fade_);
	return result;
}

ModeType::Enum ModeGame::GetType()
{
	return ModeType::Game;
}

std::vector<RenderItem> ModeGame::Draw()
{
	Vector2f grid_size(24, 24);
	const float below = -1;
	const float above = 1;
	vector<RenderItem> draw_list;
	draw_list.reserve(world_->GetEnemies().size() + 3);

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
		ri.frame_ = StandardTextures::cat_animations[p_walker->GetDirection()]->GetCurrentFrame();
		ri.depth = 0;
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
			case SquareType::NorthArrow:
			case SquareType::SouthArrow:
			case SquareType::EastArrow:
			case SquareType::WestArrow:
				{
					RenderItem ri;
					ri.position_ = Vector2f((float)x, (float)y);
					ri.frame_ = StandardTextures::arrows[SquareType::GetDirection(square_type)]->GetCurrentFrame();
					ri.depth = below;
					draw_list.push_back(ri);
				}
				break;
			case SquareType::HalfNorthArrow:
			case SquareType::HalfSouthArrow: 
			case SquareType::HalfEastArrow: 
			case SquareType::HalfWestArrow:
				{
					RenderItem ri;
					ri.position_ = Vector2f((float)x, (float)y);
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
		render_item.depth *= grid_size.y;
	}
	return std::vector<RenderItem>();
}

void ModeGame::SkillClick(Widget* _widget)
{
	Skills::Enum skill = Skills::FromString(_widget->GetTag());
	Logger::DiagnosticOut() << "Using skill " << _widget->GetTag() << ", " << skill << ", " << Skills::ToString(skill) <<"\n";
}

void ModeGame::GridClick(Widget* _widget, MouseEventArgs _args)
{
	Logger::DiagnosticOut() << "Clicked grid item " << _args.x << "," << _args.y << "\n";
}