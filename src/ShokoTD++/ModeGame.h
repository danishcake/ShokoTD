#pragma once
#include <IMode.h>
#include <Skills.h>
#include <GameGridWidget.h>
class Widget;
class ItemBrowserWidget;
class BlittableRect;
class World;
class Progression;
struct MouseEventArgs;
struct DragEventArgs;

namespace WorldState
{
	enum Enum;
}

class ModeGame :
	public IMode
{
private:
	std::vector<std::string> skills_;
	Skills::Enum selected_skill_;
	std::string level_;
	World* world_;
	WorldState::Enum ltv_world_state_;
	float dialogue_timer_;


	Widget* end_dialogue_;
	Widget* lives_left_;

	void SkillClick(Widget* _widget);
	void QuitClick(Widget* _widget);
	void GridClick(Widget* _widget, MouseEventArgs _args);
	void GridGesture(Widget* _widget, GridGestureEventArgs _args);

	Progression* progression_;
	
public:
	ModeGame(std::string _level_name, std::string _level_file, std::vector<std::string> _skills, Progression* _progression);
	virtual ~ModeGame();

	virtual IMode* Teardown();
	virtual void Setup();
	virtual ModeAction::Enum Tick(float _dt);
	virtual ModeType::Enum GetType();
	virtual std::vector<RenderItem> Draw();
};
