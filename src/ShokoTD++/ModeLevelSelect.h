#pragma once
#include <IMode.h>
#include <string>

class Widget;
class Progression;

class ModeLevelSelect :
	public IMode
{
private:
	Progression* progression_;

	void ItemClick(Widget* _widget, std::string _text);
	Widget* description_;
	Widget* stats_;
public:
	ModeLevelSelect(Progression* _progression);
	virtual IMode* Teardown();
	virtual void Setup();
	virtual ModeAction::Enum Tick(float _dt);
	virtual ModeType::Enum GetType();
	virtual std::vector<RenderItem> Draw();
	
};
