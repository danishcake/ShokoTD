#include "ModeDeckConf.h"
#include "RenderItem.h"
#include <Widget.h>
#include <ItemBrowserWidget.h>
#include <BlittableRect.h>
#include "Logger.h"
#include "ModeGame.h"
#include "Progression.h"


ModeDeckConfiguration::ModeDeckConfiguration(std::string _next_level, Progression* _progression)
{
	progression_ = _progression;
	next_level_ = _next_level;
}

IMode* ModeDeckConfiguration::Teardown()
{
	Widget::ClearRoot();
	return IMode::Teardown();
}

void ModeDeckConfiguration::Setup()
{
	std::vector<std::string> items;
	items.push_back("Arrows");
	items.push_back("Slow");
	items.push_back("Fireball");
	known_skills_ = new ItemBrowserWidget(items, Vector2i(2, 7), Vector2i(64, 64));
	known_skills_->OnItemRender.connect(boost::bind(&ModeDeckConfiguration::ItemRender, this, _1, _2, _3));
	known_skills_->PerformItemLayout();
	known_skills_->OnItemDragStart.connect(boost::bind(&ModeDeckConfiguration::ItemDragStart, this, _1, _2));
	known_skills_->OnItemClick.connect(boost::bind(&ModeDeckConfiguration::ItemClick, this, _1, _2));

	std::vector<std::string> none;
	none.push_back("Arrows");
	selected_skills_ = new ItemBrowserWidget(none, Vector2i(7, 1), Vector2i(64, 64));
	selected_skills_->SetPosition(Vector2i(128 + 20, 10));
	selected_skills_->SetAllowScroll(false);
	selected_skills_->OnItemRender.connect(boost::bind(&ModeDeckConfiguration::ItemRender, this, _1, _2, _3));
	selected_skills_->PerformItemLayout();
	
	selected_skills_->OnDragEnter.connect(boost::bind(&ModeDeckConfiguration::ItemDragEnter, this, _1, _2));
	selected_skills_->OnDragLand.connect(boost::bind(&ModeDeckConfiguration::ItemDragLand, this, _1, _2));
	
	selected_skills_->OnItemDragEnter.connect(boost::bind(&ModeDeckConfiguration::ItemDragEnter, this, _1, _2));
	selected_skills_->OnItemDragLand.connect(boost::bind(&ModeDeckConfiguration::ItemDragLand, this, _1, _2));

	Widget* next_level = new Widget("Blank96x32.png");
	next_level->SetPosition(Vector2i(534, 438));
	next_level->SetText("Done", TextAlignment::Centre);
	next_level->OnClick.connect(boost::bind(&ModeDeckConfiguration::Accept, this, _1));

	upgrades_pane_ = new Widget("Blank384x384.png");
	upgrades_pane_->SetPosition(Vector2i(148, 84));
	upgrades_pane_->SetRejectsFocus(true);
}

ModeAction::Enum ModeDeckConfiguration::Tick(float _dt)
{
	ModeAction::Enum result = IMode::Tick(_dt);
	Widget::SetFade(fade_);
	return result;
}

ModeType::Enum ModeDeckConfiguration::GetType()
{
	return ModeType::DeckConfiguration;
}

std::vector<RenderItem> ModeDeckConfiguration::Draw()
{
	return std::vector<RenderItem>();
}

void ModeDeckConfiguration::ItemRender(Widget* _widget, BlittableRect** _blittable, std::string _name)
{
	*_blittable = new BlittableRect(_name + "_icon.png");
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
	pend_mode_ = new ModeGame(next_level_, selected_skills_->GetItems(), progression_);
}

void ModeDeckConfiguration::ItemClick(Widget* _widget, std::string _item)
{
	Logger::DiagnosticOut() << _item << "\n";
}