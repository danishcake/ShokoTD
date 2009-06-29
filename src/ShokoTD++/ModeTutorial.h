#pragma once
#include <IMode.h>
class Widget;

class ModeTutorial :
	public IMode
{
private:
	Widget* next_;
	Widget* finish_;

	Widget* page1_;
	void NextClick(Widget* _widget);
	void FinishClick(Widget* _widget);
public:
	ModeTutorial();
	virtual IMode* Teardown();
	virtual void Setup();
	virtual ModeAction::Enum Tick(float _dt);
	virtual ModeType::Enum GetType();
	virtual std::vector<RenderItem> Draw();
};
