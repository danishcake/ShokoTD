#include "ModeMenu.h"
#include "RenderItem.h"
#include <Widget.h>
#include "ModeTutorial.h"

ModeMenu::ModeMenu()
{
	exitting_ = false;
}

IMode* ModeMenu::Teardown()
{
	Widget::ClearRoot();
	return IMode::Teardown();
}

void ModeMenu::Setup()
{
	Widget* newGame = new Widget("Blank128x32.png"); 
	newGame->SetText("New game", TextAlignment::Centre);
	newGame->SetPosition(Vector2i(10, 20));
	newGame->OnClick.connect(boost::bind(&ModeMenu::NewGameClick, this, _1));
	
	Widget* loadGame = new Widget("Blank128x32.png");
	loadGame->SetText("Load", TextAlignment::Centre);
	loadGame->SetPosition(Vector2i(10, 62));
	loadGame->OnClick.connect(boost::bind(&ModeMenu::LoadClick, this, _1));

	Widget* options = new Widget("Blank128x32.png");
	options->SetText("Options", TextAlignment::Centre);
	options->SetPosition(Vector2i(10, 104));
	options->OnClick.connect(boost::bind(&ModeMenu::OptionsClick, this, _1));

	Widget* unlocks = new Widget("Blank128x32.png");
	unlocks->SetText("Unlocks", TextAlignment::Centre);
	unlocks->SetPosition(Vector2i(10, 146));
	unlocks->OnClick.connect(boost::bind(&ModeMenu::UnlocksClick, this, _1));

	Widget* exit = new Widget("Blank128x32.png");
	exit->SetText("Exit", TextAlignment::Centre);
	exit->SetPosition(Vector2i(10, 188));
	exit->OnClick.connect(boost::bind(&ModeMenu::ExitClick, this, _1));

	not_done_yet_ = new Widget("Blank600x200.png");
	not_done_yet_->SetPosition(Vector2i(-700, 0));
	not_done_yet_->SetText("Feature completion?\nBugger all chance of that, lad.\nA haiku instead.", TextAlignment::TopLeft);
	not_done_yet_->SetRejectsFocus(true);

	Widget* dismiss = new Widget("Blank128x32.png");
	dismiss->SetPosition(Vector2i(462, 158));
	dismiss->OnClick.connect(boost::bind(&ModeMenu::DismissClick, this, _1));
	dismiss->SetText("Back", TextAlignment::Centre);
	not_done_yet_->AddChild(dismiss);
}

ModeAction::Enum ModeMenu::Tick(float _dt)
{
	ModeAction::Enum result = ModeAction::NoAction;
	result = IMode::Tick(_dt);
	if(!exitting_)
	{
		Widget::SetFade(fade_);
	} else
	{
		Widget::SetFade((age_ - pend_time_) / pend_time_);
		if(age_ - pend_time_ >= pend_time_)
			result = ModeAction::Exit;
	}

	return result;
}

ModeType::Enum ModeMenu::GetType()
{
	return ModeType::Menu;
}

std::vector<RenderItem> ModeMenu::Draw()
{
return std::vector<RenderItem>();
}

/* Event handlers */
void ModeMenu::NewGameClick(Widget* _widget)
{
	if(!pend_mode_)
	pend_mode_ = new ModeTutorial();
}

void ModeMenu::LoadClick(Widget* _widget)
{
	NotDoneYet();
}

void ModeMenu::OptionsClick(Widget* _widget)
{
	NotDoneYet();
}

void ModeMenu::UnlocksClick(Widget* _widget)
{
	NotDoneYet();
}

void ModeMenu::ExitClick(Widget* _widget)
{
	if(!exitting_)
	{
		exitting_ = true;
		age_ = pend_time_;
	}
}

void ModeMenu::NotDoneYet()
{
	not_done_yet_->SetPosition(Vector2i(320 - 300, 240 - 100));
	not_done_yet_->SetModal(true);
}
void ModeMenu::DismissClick(Widget* _widget)
{
	not_done_yet_->SetPosition(Vector2i(-700, 0));
	not_done_yet_->SetModal(false);
}