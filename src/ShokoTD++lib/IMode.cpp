#include "IMode.h"
#include <cstring> //For NULL
#include <assert.h>

IMode::IMode()
{
	pend_mode_ = NULL;
	pend_time_ = 1;
	pend_timer_ = 0;
	age_ = 0;
	fade_ = 0;
}

ModeAction::Enum IMode::Tick(float _dt)
{
	if(pend_mode_)
		pend_timer_ += _dt;
	else
		pend_timer_ -= _dt;

	pend_timer_ = pend_timer_ < 0 ? 0 : pend_timer_;
	age_ += _dt;
	
	float fade_in = 1.0f - (age_ / pend_time_);
	float fade_out = pend_timer_ / pend_time_;
	fade_ = fade_in > fade_out ? fade_in : fade_out;
	fade_ = fade_ > 1 ? 1 : fade_;
	
	if(pend_timer_ > pend_time_)
		return ModeAction::ChangeMode;
	else
		return ModeAction::NoAction;
}

IMode* IMode::Teardown()
{
	assert(pend_mode_);
	return pend_mode_;
}