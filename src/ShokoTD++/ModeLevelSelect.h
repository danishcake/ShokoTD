#pragma once
#include <IMode.h>
#include <string>

class Widget;
class Progression;
class BlittableRect;

class ModeLevelSelect :
	public IMode
{
private:
	Progression* progression_;
	std::string last_selected_level_;

	Widget* description_;
	Widget* stats_;

	void ItemClick(Widget* _widget, std::string _text);
	void ReturnToMenuClick(Widget* _widget);
	void PlayLevelClick(Widget* _widget);
	void ItemRender(Widget* _widget, BlittableRect** _rect, std::string _text);

public:
	ModeLevelSelect(Progression* _progression);
	virtual IMode* Teardown();
	virtual void Setup();
	virtual ModeAction::Enum Tick(float _dt);
	virtual ModeType::Enum GetType();
	virtual std::vector<RenderItem> Draw();
	
};
