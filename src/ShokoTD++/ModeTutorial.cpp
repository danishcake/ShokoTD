#include "ModeTutorial.h"
#include "RenderItem.h"
//#include "ModeDeckConf.h"
#include "ModeLevelSelect.h"
#include <Widget.h>
#include <Progression.h>

ModeTutorial::ModeTutorial()
{
	finish_ = NULL;
}

IMode* ModeTutorial::Teardown()
{
	Widget::ClearRoot();
	return IMode::Teardown();
}

void ModeTutorial::Setup()
{
	next_ = new Widget("Blank128x32.png");
	next_->SetPosition(Vector2i(640 - 256 - 20, 480 - 32 - 10));
	next_->SetText("Next", TextAlignment::Centre);
	next_->OnClick.connect(boost::bind(&ModeTutorial::NextClick, this, _1));
}

ModeAction::Enum ModeTutorial::Tick(float _dt)
{
	ModeAction::Enum result = IMode::Tick(_dt);
	Widget::SetFade(fade_);
	return result;
}

ModeType::Enum ModeTutorial::GetType()
{
	return ModeType::Tutorial;
}

std::vector<RenderItem> ModeTutorial::Draw()
{
	return std::vector<RenderItem>();
}

/* Event handlers */
void ModeTutorial::NextClick(Widget* _widget)
{
	if(!finish_)
	{
		finish_ = new Widget("Blank128x32.png");
		finish_->SetText("Finish", TextAlignment::Centre);
		finish_->SetPosition(Vector2i(640 - 128 - 10, 480 - 32 - 10));
		finish_->OnClick.connect(boost::bind(&ModeTutorial::FinishClick, this, _1));
	}
}

void ModeTutorial::FinishClick(Widget* _widget)
{
	pend_mode_ = new ModeLevelSelect(new Progression("ToHellAndBack.Campaign", ""));
}
