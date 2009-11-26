#include "ShokoTDGSM.h"
#include "ModeIntro.h"
#include "ModeMenu.h"
#include <vmath.h>
#include <algorithm>
#include <boost/foreach.hpp>
#include "SDLAnimationFrame.h"
#include "RenderItem.h"

ShokoTDGSM::ShokoTDGSM(void)
{

}

void ShokoTDGSM::Init()
{
	game_mode_ = new ModeIntro();
	game_mode_->Setup();
}

ShokoTDGSM::~ShokoTDGSM(void)
{
}


bool ShokoTDGSM::Tick(float _timespan)
{
	ModeAction::Enum action = game_mode_->Tick(_timespan);
	if(action == ModeAction::ChangeMode)
	{
		IMode* pend_mode = game_mode_->Teardown();
		delete game_mode_;
		game_mode_ = pend_mode;
		game_mode_->Setup();
	} else if(action == ModeAction::Exit)
	{
		return true;
	}
	return false;
}

void ShokoTDGSM::Draw(SDL_Surface* _target)
{
	std::vector<RenderItem> drawlist = game_mode_->Draw();
	//Sort front to back to prevent overlay issues
	std::sort(drawlist.begin(), drawlist.end(), RenderItem::DepthSort<RenderItem>());
	SDLAnimationFrame::screen_ = _target;
	
	BOOST_FOREACH(RenderItem& ri, drawlist)
	{
		ri.position_ += Vector2f(2, 68);
		ri.frame_->Draw(ri.position_);
	}
}