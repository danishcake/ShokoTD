#include "ModeLevelSelect.h"
#include "ModeMenu.h"
#include "ModeDeckConf.h"
#include "RenderItem.h"
#include <Widget.h>
#include <ItemBrowserWidget.h>
#include <vector>
#include <ProgressLevel.h>
#include <Progression.h>


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
	std::vector<ProgressLevel*> openlevels = progression_->GetUnlocked();
	std::vector<ProgressLevel*> lockedlevels = progression_->GetLocked();
	for(std::vector<ProgressLevel*>::iterator it = openlevels.begin(); it != openlevels.end(); ++it)
	{
		levels.push_back((*it)->GetFilename());
	}
	for(std::vector<ProgressLevel*>::iterator it = lockedlevels.begin(); it != lockedlevels.end(); ++it)
	{
		levels.push_back("Locked");
	}

	ItemBrowserWidget* level_browse = new ItemBrowserWidget(levels, Vector2i(6,6), Vector2i(64, 64));
	level_browse->SetPosition(Vector2i(10, 10));
	level_browse->OnItemClick.connect(boost::bind(&ModeLevelSelect::ItemClick, this, _1, _2));
	level_browse->OnItemRender.connect(boost::bind(&ModeLevelSelect::ItemRender, this, _1, _2, _3));
	level_browse->PerformItemLayout();

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
	play->OnClick.connect(boost::bind(&ModeLevelSelect::PlayLevelClick, this, _1));
	Widget* quit = new Widget("Blank96x32.png");
	quit->SetPosition(Vector2i(510, 436));
	quit->SetText("Quit", TextAlignment::Centre);
	quit->OnClick.connect(boost::bind(&ModeLevelSelect::ReturnToMenuClick, this, _1));
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
	last_selected_level_ = _text;
}

void ModeLevelSelect::ReturnToMenuClick(Widget* _widget)
{
	if(!pend_mode_)
		pend_mode_ = new ModeMenu();
}

void ModeLevelSelect::PlayLevelClick(Widget* _widget)
{
	if(last_selected_level_ != "" && last_selected_level_ != "Locked")
	{
		if(!pend_mode_)
			pend_mode_ = new ModeDeckConfiguration(last_selected_level_, progression_);
	}
}

void ModeLevelSelect::ItemRender(Widget* _widget, BlittableRect** _rect, std::string _text)
{
	if(_text == "Locked")
		*_rect = new BlittableRect("Locked.png");
}

