#include "ModeIntro.h"
#include "ModeMenu.h"
#include "RenderItem.h"
#include <Widget.h>


IMode* ModeIntro::Teardown()
{
	Widget::ClearRoot();
	return IMode::Teardown();
}

void ModeIntro::Setup()
{
	Widget* splash = new Widget("Blank128x32.png");
	splash->SetPosition(Vector2i(640 - 128 - 10, 480 - 32 - 10));
	splash->SetText("Start!", TextAlignment::Centre);
	splash->OnClick.connect(boost::bind(&ModeIntro::IntroClick, this, _1));	
}

ModeType::Enum ModeIntro::GetType()
{
	return ModeType::Intro;
}

ModeAction::Enum ModeIntro::Tick(float _dt)
{
	ModeAction::Enum result = IMode::Tick(_dt);
	Widget::SetFade(fade_);
	return result;
}

std::vector<RenderItem> ModeIntro::Draw()
{
	return std::vector<RenderItem>();	
}

/* Event handlers */
void ModeIntro::IntroClick(Widget* _widget)
{
	if(!pend_mode_)
		pend_mode_ = new ModeMenu();
}