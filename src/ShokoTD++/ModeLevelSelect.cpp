#include "ModeLevelSelect.h"
#include "RenderItem.h"
#include <Widget.h>
#include <ItemBrowserWidget.h>
#include <vector>

ModeLevelSelect::ModeLevelSelect(Progression* _progression)
{
	progression_ = _progression;
}

IMode* ModeLevelSelect::Teardown()
{
	Widget::ClearRoot();
	return IMode::Teardown();
}

void ModeLevelSelect::Setup()
{
	std::vector<std::string> levels;
	levels.push_back("Level 1.level");
	ItemBrowserWidget* level_browse = new ItemBrowserWidget(levels, Vector2i(6,6), Vector2i(64, 64));
	level_browse->SetPosition(Vector2i(10, 10));
	level_browse->OnItemClick.connect(boost::bind(&ModeLevelSelect::ItemClick, this, _1, _2));

	description_ = new Widget("Blank384x66.png");
	description_->SetPosition(Vector2i(10, 404));
	description_->SetText("Description", TextAlignment::Centre);
	description_->SetRejectsFocus(true);
	
	stats_ = new Widget("Blank226x384.png");
	stats_->SetPosition(Vector2i(404, 10));
	stats_->SetRejectsFocus(true);

	Widget* play = new Widget("Blank96x32.png");
	play->SetPosition(Vector2i(404, 436));
	play->SetText("Play", TextAlignment::Centre);
	Widget* quit = new Widget("Blank96x32.png");
	quit->SetPosition(Vector2i(510, 436));
	quit->SetText("Quit", TextAlignment::Centre);
}

ModeType::Enum ModeLevelSelect::GetType()
{
	return ModeType::LevelSelect;
}

ModeAction::Enum ModeLevelSelect::Tick(float _dt)
{
	ModeAction::Enum result = IMode::Tick(_dt);
	Widget::SetFade(fade_);
	return result;
}

std::vector<RenderItem> ModeLevelSelect::Draw()
{
	return std::vector<RenderItem>();
}

/* Event handlers */

void ModeLevelSelect::ItemClick(Widget* _widget, std::string _text)
{
	description_->SetText(_text, TextAlignment::TopLeft);
}
