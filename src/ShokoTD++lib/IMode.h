#pragma once
#include <vector>
struct RenderItem;

namespace ModeType
{
	enum Enum
	{
		Intro,
		Menu,
		CampaignSelect,
		Tutorial,
		LevelSelect,
		DeckConfiguration,
		Game,
	};
}

namespace ModeAction
{
	enum Enum
	{
		NoAction,
		ChangeMode,
		Exit
	};
}

/*
 * Represents a mode for a state machine.
 */
class IMode
{
protected:
	IMode* pend_mode_;
	float pend_time_;
	float pend_timer_;
	float age_;
	float fade_;
public:
	IMode();
	virtual ~IMode(){}
	virtual IMode* Teardown();
	virtual void Setup() = 0;
	virtual ModeAction::Enum Tick(float _dt);
	virtual ModeType::Enum GetType() = 0;
	virtual std::vector<RenderItem> Draw() = 0;
};
