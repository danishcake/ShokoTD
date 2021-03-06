#include "ModeCampaignSelect.h"
#include "ModeMenu.h"
#include "ModeLevelSelect.h"
#include "RenderItem.h"
#include <Widget.h>
#include <ItemBrowserWidget.h>
#include <vector>
#include <ProgressLevel.h>
#include <Progression.h>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <World.h>


ModeCampaignSelect::ModeCampaignSelect()
{
}

IMode* ModeCampaignSelect::Teardown()
{
	Widget::ClearRoot();
	return IMode::Teardown();
}

void ModeCampaignSelect::Setup()
{

	Widget* start_campaign = new Widget("Blank96x32.png");
	start_campaign->SetPosition(Vector2i(640 - 96 - 10, 480 - 32 - 10));
	start_campaign->SetText("Start", TextAlignment::Centre);
	start_campaign->OnClick.connect(boost::bind(&ModeCampaignSelect::StartCampaignClick, this, _1));

	resume_campaign_ = new Widget("Blank96x32.png");
	resume_campaign_->SetVisibility(false);
	resume_campaign_->SetPosition(Vector2i(640 - 96*2 - 20, 480 - 32 - 10));
	resume_campaign_->SetText("Resume", TextAlignment::Centre);
	resume_campaign_->OnClick.connect(boost::bind(&ModeCampaignSelect::ResumeCampaignClick, this, _1));


	Widget* go_back = new Widget("Blank96x32.png");
	go_back->SetPosition(Vector2i(10, 480 - 32 - 10));
	go_back->SetText("Back", TextAlignment::Centre);
	go_back->OnClick.connect(boost::bind(&ModeCampaignSelect::ReturnToMenuClick, this, _1));

	campaign_description_ = new Widget("Blank600x200.png");
	campaign_description_->SetPosition(Vector2i(20, 84));
	campaign_description_->SetText("Select a campaign", TextAlignment::TopLeft);
	campaign_description_->SetRejectsFocus(true);
	campaign_description_->SetTextWrap(true);
	std::vector<std::string> campaigns;
	/* Load a list of campaigns */
	boost::filesystem::directory_iterator end_itr;	
	for(boost::filesystem::directory_iterator itr = boost::filesystem::directory_iterator("./Levels");
		itr != end_itr;
		++itr)
	{
		if(boost::filesystem::is_regular((itr->status())))
		{
			std::string ext = boost::filesystem::extension(*itr);
			if(ext == ".Campaign")
			{
				campaigns.push_back(boost::filesystem::basename(itr->path()));
			}
		}
	}

	ItemBrowserWidget* campaign_browse = new ItemBrowserWidget(campaigns, Vector2i(9,1), Vector2i(64, 64));
	campaign_browse->SetPosition(Vector2i(32, 10));
	campaign_browse->OnItemClick.connect(boost::bind(&ModeCampaignSelect::ItemClick, this, _1, _2));
	campaign_browse->OnItemRender.connect(boost::bind(&ModeCampaignSelect::ItemRender, this, _1, _2, _3));
	campaign_browse->PerformItemLayout();
}

ModeType::Enum ModeCampaignSelect::GetType()
{
	return ModeType::CampaignSelect;
}

ModeAction::Enum ModeCampaignSelect::Tick(float _dt)
{
	ModeAction::Enum result = IMode::Tick(_dt);
	Widget::SetFade(fade_);
	return result;
}

std::vector<RenderItem> ModeCampaignSelect::Draw()
{
	return std::vector<RenderItem>();
}

/* Event handlers */
void ModeCampaignSelect::ItemClick(Widget* /*_widget*/, std::string _text)
{
	selected_campaign_ = _text;
	if(boost::filesystem::exists(selected_campaign_ + ".save"))
	{
		resume_campaign_->SetVisibility(true);
	} else
	{
		resume_campaign_->SetVisibility(false);
	}
	Progression* campaign_prog = new Progression(selected_campaign_ + ".Campaign", selected_campaign_ + ".Save", true);
    CampaignDescription desc = campaign_prog->GetDescription();

		
	campaign_prog->DisableSave();
	delete campaign_prog;
	campaign_description_->SetText(desc.name + "\n\n" +desc.description, TextAlignment::TopLeft);
	
}

void ModeCampaignSelect::ReturnToMenuClick(Widget* /*_widget*/)
{
	if(!pend_mode_)
		pend_mode_ = new ModeMenu();
}

void ModeCampaignSelect::StartCampaignClick(Widget* /*_widget*/)
{
	if(!pend_mode_ && selected_campaign_.length() > 0)
		pend_mode_ = new ModeLevelSelect(new Progression(selected_campaign_ + ".Campaign", selected_campaign_ + ".save", false));
}

void ModeCampaignSelect::ResumeCampaignClick(Widget* _widget)
{
	if(!pend_mode_ && selected_campaign_.length() > 0 && _widget->GetVisibility())
		pend_mode_ = new ModeLevelSelect(new Progression(selected_campaign_ + ".Campaign", selected_campaign_ + ".save", true));
}

void ModeCampaignSelect::ItemRender(Widget* /*_widget*/, BlittableRect** _rect, std::string _text)
{
	std::string icon_filename = "Levels/" + Progression::GetCampaignIcon(_text);
	*_rect = new BlittableRect(icon_filename, true);
}

