#pragma once
#include <IMode.h>
#include <vmath.h>

class Widget;
class AnimationFrame;

class ModeIntro :
	public IMode
{
private:
	void IntroClick(Widget* _widget);
	Widget* logo_;
	Widget* logo2_;
	Vector2f logo_position_;
	Vector2f logo2_position_;
public:
	virtual IMode* Teardown();
	virtual void Setup();
	virtual ModeAction::Enum Tick(float _dt);
	virtual ModeType::Enum GetType();
	virtual std::vector<RenderItem> Draw();
	
};
