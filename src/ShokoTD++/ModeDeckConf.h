#pragma once
#include <IMode.h>
class Widget;
class ItemBrowserWidget;
class BlittableRect;
struct DragEventArgs;
class Progression;

class ModeDeckConfiguration :
	public IMode
{
private:
	void ItemRender(Widget* _widget, BlittableRect** _blittable, std::string);
	void ItemDragStart(Widget* _widget, DragEventArgs* _drag_args);
	void ItemDragEnter(Widget* _widget, DragEventArgs* _drag_args);
	void ItemDragLand(Widget* _widget, DragEventArgs* _drag_args);

	void ItemClick(Widget* _widget, std::string _item);

	void Accept(Widget* _widget);

	ItemBrowserWidget* known_skills_;
	ItemBrowserWidget* selected_skills_;
	Widget* upgrades_pane_;

	std::string next_level_;
	Progression* progression_;

public:
	ModeDeckConfiguration(std::string _next_level, Progression* _progression);

	virtual IMode* Teardown();
	virtual void Setup();
	virtual ModeAction::Enum Tick(float _dt);
	virtual ModeType::Enum GetType();
	virtual std::vector<RenderItem> Draw();
};
