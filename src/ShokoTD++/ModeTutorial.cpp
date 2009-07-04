#include "ModeTutorial.h"
#include "RenderItem.h"
//#include "ModeDeckConf.h"
#include "ModeLevelSelect.h"
#include <Widget.h>
#include <Progression.h>

ModeTutorial::ModeTutorial()
{
}

IMode* ModeTutorial::Teardown()
{
	Widget::ClearRoot();
	return IMode::Teardown();
}

void ModeTutorial::Setup()
{
	next_ = new Widget("Blank128x32.png");
	next_->SetPosition(Vector2i(640 - 128 - 10, 480 - 32 - 10));
	next_->SetText("Next", TextAlignment::Centre);
	next_->OnClick.connect(boost::bind(&ModeTutorial::NextClick, this, _1));
	
	page_ = new Widget("Instructions.png");
	page_->SetPosition(Vector2i(10, 10));
	page_->SetRejectsFocus(true);
	page_index_ = 0;	
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
	page_index_++;
	switch(page_index_)
	{
	case 1:
		delete page_;
		page_ = new Widget("Instructions2.png");
		page_->SetRejectsFocus(true);
		break;
	case 2:
		delete page_;
		page_ = new Widget("Instructions3.png");
		page_->SetRejectsFocus(true);
		break;
	case 3:
		delete page_;
		page_ = new Widget("Instructions4.png");
		page_->SetRejectsFocus(true);
		break;
	case 4:
		delete page_;
		page_ = new Widget("Instructions5.png");
		page_->SetRejectsFocus(true);
		next_->SetText("Finish", TextAlignment::Centre);
		break;
	default:
		if(pend_mode_ == NULL)
			pend_mode_ = new ModeLevelSelect(new Progression("ToHellAndBack.Campaign", ""));
		break;
	}
}

