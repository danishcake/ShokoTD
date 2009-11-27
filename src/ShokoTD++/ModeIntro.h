#pragma once
#include <IMode.h>
#include <vmath.h>
#include <boost/signal.hpp>

class Widget;
class AnimationFrame;
class KeyPressEventArgs;

class ModeIntro :
	public IMode
{
private:
	void IntroClick(Widget* _widget);
	Widget* logo_;
	Widget* logo2_;
	Vector2f logo_position_;
	Vector2f logo2_position_;

	boost::signals::scoped_connection  keypress_connection_;
	void Keypress(Widget* _widget, KeyPressEventArgs _args);
public:
	virtual IMode* Teardown();
	virtual void Setup();
	virtual ModeAction::Enum Tick(float _dt);
	virtual ModeType::Enum GetType();
	virtual std::vector<RenderItem> Draw();
};
