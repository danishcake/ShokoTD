#pragma once
#include <IMode.h>
class Widget;

class ModeTutorial :
	public IMode
{
private:
	Widget* next_;

	int page_index_;
	Widget* page_;
	void NextClick(Widget* _widget);
public:
	ModeTutorial();
	virtual IMode* Teardown();
	virtual void Setup();
	virtual ModeAction::Enum Tick(float _dt);
	virtual ModeType::Enum GetType();
	virtual std::vector<RenderItem> Draw();
};
