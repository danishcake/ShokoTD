#pragma once
#include <IMode.h>
class Widget;

class ModeMenu :
	public IMode
{
private:
	void NewGameClick(Widget* _widget);
	void LoadClick(Widget* _widget);
	void OptionsClick(Widget* _widget);
	void UnlocksClick(Widget* _widget);
	void ExitClick(Widget* _widget);

	Widget* not_done_yet_;
	void NotDoneYet();
	void DismissClick(Widget* _widget);

	bool exitting_;
public:
	ModeMenu();
	virtual IMode* Teardown();
	virtual void Setup();
	virtual ModeAction::Enum Tick(float _dt);
	virtual ModeType::Enum GetType();
	virtual std::vector<RenderItem> Draw();
};
