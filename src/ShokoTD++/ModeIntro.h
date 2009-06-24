#pragma once
#include <IMode.h>

class Widget;

class ModeIntro :
	public IMode
{
private:
	void IntroClick(Widget* _widget);
public:
	virtual IMode* Teardown();
	virtual void Setup();
	virtual ModeAction::Enum Tick(float _dt);
	virtual ModeType::Enum GetType();
	virtual std::vector<RenderItem> Draw();
	
};
