#pragma once
#include <IMode.h>
#include <string>

class Widget;
class BlittableRect;

class ModeCampaignSelect :
	public IMode
{
private:
	void ItemClick(Widget* _widget, std::string _text);
	void ReturnToMenuClick(Widget* _widget);
	void StartCampaignClick(Widget* _widget);
	void ResumeCampaignClick(Widget* _widget);
	void ItemRender(Widget* _widget, BlittableRect** _rect, std::string _text);
	std::string selected_campaign_;
	Widget* campaign_description_;
	Widget* resume_campaign_;

public:
	ModeCampaignSelect();
	virtual IMode* Teardown();
	virtual void Setup();
	virtual ModeAction::Enum Tick(float _dt);
	virtual ModeType::Enum GetType();
	virtual std::vector<RenderItem> Draw();
};
