#pragma once
#include <IMode.h>
#include <Skills.h>
#include <GameGridWidget.h>
#include <map>
class Widget;
class ItemBrowserWidget;
class BlittableRect;
class World;
class Progression;
struct MouseEventArgs;
struct DragEventArgs;
class Animation;

namespace WorldState
{
	enum Enum;
}

namespace Direction
{
	enum Enum;
}

struct Decoration
{
	Vector2i position;
	Animation* animation;
	float time_to_live;

	static bool IsDead(Decoration _deco)
	{
		return _deco.time_to_live < 0;
	}
};

class ModeGame :
	public IMode
{
private:
	std::vector<Decoration> decorations_;
	std::vector<std::string> skills_;
	Skills::Enum selected_skill_;
	std::string level_;
	World* world_;
	WorldState::Enum ltv_world_state_;
	float dialogue_timer_;
	Vector2i last_grid_position_;


	Widget* end_dialogue_;
	Widget* lives_left_;

	void SkillClick(Widget* _widget);
	void QuitClick(Widget* _widget);
	void GridClick(Widget* _widget, MouseEventArgs _args);
	void GridMove(Widget* _widget, MouseEventArgs _args);
	boost::signals::scoped_connection  keypress_connection_;
	void Keypress(Widget* _widget, KeyPressEventArgs _args);
	void GridGesture(Widget* _widget, GridGestureEventArgs _args);

	Progression* progression_;

	/* Skill firing */
	typedef std::map<std::string, float> cooldown_t;
	cooldown_t cooldowns_;
	bool CooldownOK(std::string _skillname);
	void DoArrows(Vector2i _position, Direction::Enum _direction);
	void DoBurn(Vector2i _position);
	void DoSlow(Vector2i _position);
	void DoCraze(Vector2i _position);
	void DoSpawnPause();
	void DoPermaSlow();

	
public:
	ModeGame(std::string _level_name, std::string _level_file, Progression* _progression);
	virtual ~ModeGame();

	virtual IMode* Teardown();
	virtual void Setup();
	virtual ModeAction::Enum Tick(float _dt);
	virtual ModeType::Enum GetType();
	virtual std::vector<RenderItem> Draw();
};
