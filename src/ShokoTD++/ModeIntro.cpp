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
	splash->SetText("Skip!", TextAlignment::Centre);
	splash->OnClick.connect(boost::bind(&ModeIntro::IntroClick, this, _1));

	logo_ = new Widget("gamedevchallenge2.png");
	logo_position_ = Vector2f(-(static_cast<float>(logo_->GetSize().x) + 100.0f), (480.0f - static_cast<float>(logo_->GetSize().y) ) / 2.0f);
	logo_->SetRejectsFocus(true);
	
	logo_->SetPosition(logo_position_);


	logo2_ = new Widget("logo.png");
	logo2_position_ = Vector2f(-(static_cast<float>(logo2_->GetSize().x) + 100.0f), (480.0f - static_cast<float>(logo2_->GetSize().y) ) / 2.0f);
	logo2_->SetRejectsFocus(true);
	logo2_->SetPosition(logo2_position_);
	
	keypress_connection_ = Widget::OnGlobalKeyUp.connect(boost::bind(&ModeIntro::Keypress, this, _1, _2));
}

ModeType::Enum ModeIntro::GetType()
{
	return ModeType::Intro;
}

ModeAction::Enum ModeIntro::Tick(float _dt)
{
	static const float intro_time_0 = 0.5f;
	static const float intro_time_1 = 1.0f;
	static const float intro_time_2 = 2.0f;
	static const float intro_time_3 = 2.5f;
	static const float intro_time_4 = 3.7f;
	static const float intro_time_5 = 5.7f;
	static const float intro_time_6 = 6.0f;

	float pos_frac = 0;
	float pos_frac2 = -1;
	if(age_ < intro_time_0)
	{
		pos_frac = -1;
	} else if(age_ < intro_time_1)
	{
		pos_frac = -1.0f + (age_ - intro_time_0) / (intro_time_1 - intro_time_0);
	} else if(age_ < intro_time_2)
	{
		pos_frac = 0;
	} else if(age_ < intro_time_3)
	{
		pos_frac = (age_ - intro_time_2) / (intro_time_3 - intro_time_2);
	} else if(age_ < intro_time_4)
	{
		pos_frac = 1;
		pos_frac2 = -1.0f + (age_ - intro_time_3) / (intro_time_4 - intro_time_3);
	} else
	{
		pos_frac = 1;
		pos_frac2 = 0;
	}

	pos_frac = pos_frac * fabs(pos_frac);
	pos_frac2 = pos_frac2 * fabs(pos_frac2);

	logo_position_ = Vector2f(320.0f - static_cast<float>(logo_->GetSize().x) / 2.0f, 240.0f - static_cast<float>(logo_->GetSize().y) / 2.0f);
	logo_position_ += Vector2f(400.0f + static_cast<float>(logo_->GetSize().x), 0) * pos_frac;
	logo_->SetPosition(logo_position_);

	logo2_position_ = Vector2f(320.0f - static_cast<float>(logo2_->GetSize().x) / 2.0f , 240.0f - static_cast<float>(logo2_->GetSize().y) / 2.0f);
	logo2_position_ += Vector2f(0, 250.0f + static_cast<float>(logo_->GetSize().y)) * pos_frac2;
	logo2_->SetPosition(logo2_position_);

	if(!pend_mode_ && age_ > intro_time_6)
		pend_mode_ = new ModeMenu();


	ModeAction::Enum result = IMode::Tick(_dt);
	Widget::SetFade(fade_);
	return result;
}

std::vector<RenderItem> ModeIntro::Draw()
{
	return std::vector<RenderItem>();	

}

/* Event handlers */
void ModeIntro::IntroClick(Widget* /*_widget*/)
{
	if(!pend_mode_)
		pend_mode_ = new ModeMenu();
}

void ModeIntro::Keypress(Widget* /*_widget*/, KeyPressEventArgs _args)
{
	if(!pend_mode_ && (_args.key_code == SDLK_ESCAPE || _args.key_code == SDLK_RETURN ||
		_args.key_code == SDLK_SPACE))
	{
		pend_mode_ = new ModeMenu();
	}
}