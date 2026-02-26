#include "Widgets/DialogMissionGoals.h"

#include "MissionGoal.h"
#include "MissionGoalsTracker.h"
#include <Widgets/GameButton.h>
#include "Widgets/GameUIData.h"
#include "Widgets/ProgressBarObjectVisualStat.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/GraphicConstants.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/ImageButton.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringManager.h>

#include <iomanip>
#include <sstream>

// anonymous namespace for local "private" classes
namespace
{

using namespace game;

// ====== BUTTON CLOSE =====
class ButtonCloseDMG : public sgl::sgui::ImageButton
{
public:
    ButtonCloseDMG(sgl::sgui::Widget * parent)
        : sgl::sgui::ImageButton({
                                    ID_DLG_MGOALS_BTN_CLOSE_NORMAL,
                                    ID_DLG_MGOALS_BTN_CLOSE_DISABLED,
                                    ID_DLG_MGOALS_BTN_CLOSE_OVER,
                                    ID_DLG_MGOALS_BTN_CLOSE_PUSHED,
                                    ID_DLG_MGOALS_BTN_CLOSE_NORMAL
                                 },
                                 SpriteFileDialogMissionGoals, parent)
    {
        SetShortcutKey(sgl::core::KeyboardEvent::KEY_ESCAPE);
    }

private:
    void HandleMouseOver() override
    {
        sgl::sgui::AbstractButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-02.ogg");
    }

    void HandleButtonDown() override
    {
        sgl::sgui::AbstractButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_click-02.ogg");
    }
};

// ====== BUTTON END =====
class ButtonEndMission : public GameButton
{
public:
    ButtonEndMission(sgl::sgui::Widget * parent)
        : GameButton(SpriteFileDialogMissionGoals,
                     { ID_DLG_MGOALS_BTN_COLLECT_NORMAL, ID_DLG_MGOALS_BTN_COLLECT_DISABLED,
                       ID_DLG_MGOALS_BTN_COLLECT_OVER, ID_DLG_MGOALS_BTN_COLLECT_PUSHED,
                       ID_DLG_MGOALS_BTN_COLLECT_NORMAL },
                     { 0xd7f4deff, 0x436f4dff, 0xebf9eeff, 0xc3eeceff, 0xc3eeceff}, parent)
    {
        using namespace sgl;

        // LABEL
        auto fm = graphic::FontManager::Instance();
        const int size = 18;

        auto font = fm->GetFont(WidgetsConstants::FontFileButton, size, graphic::Font::NORMAL);
        SetLabelFont(font);

        auto sm = utilities::StringManager::Instance();
        SetLabel(sm->GetCString("END_MISSION"));
    }

private:
    void HandleMouseOver() override
    {
        GameButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-02.ogg");
    }

    void HandleButtonDown() override
    {
        GameButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_click-02.ogg");
    }
};

// ====== BUTTON COLLECT =====
class ButtonCollect : public GameButton
{
public:
    ButtonCollect(sgl::sgui::Widget * parent)
        : GameButton(SpriteFileDialogMissionGoals,
                     { ID_DLG_MGOALS_BTN_COLLECT_NORMAL, ID_DLG_MGOALS_BTN_COLLECT_DISABLED,
                       ID_DLG_MGOALS_BTN_COLLECT_OVER, ID_DLG_MGOALS_BTN_COLLECT_PUSHED,
                       ID_DLG_MGOALS_BTN_COLLECT_NORMAL },
                     { 0xc3dfeeff, 0x4d6673ff, 0xebf4f9ff, 0xc3dfeeff, 0xc3dfeeff }, parent)
    {
        using namespace sgl;

        // LABEL
        auto fm = graphic::FontManager::Instance();
        const int size = 18;

        auto font = fm->GetFont(WidgetsConstants::FontFileButton, size, graphic::Font::NORMAL);
        SetLabelFont(font);

        auto sm = utilities::StringManager::Instance();
        SetLabel(sm->GetCString("COLLECT"));
    }

private:
    void HandleMouseOver() override
    {
        GameButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-02.ogg");
    }

    void HandleButtonDown() override
    {
        GameButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_click-02.ogg");
    }
};

} // namespace

namespace game
{

// ===== DIALOG =====
DialogMissionGoals::DialogMissionGoals(MissionGoalsTracker * mgt)
    : mTrackerMG(mgt)
{
    using namespace sgl;

    const std::vector<MissionGoal> & goals = mTrackerMG->GetGoals();

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    auto sm = utilities::StringManager::Instance();

    // -- define content height based on goals --
    int contentH = 0;

    graphic::Texture * texRow1 = tm->GetSprite(SpriteFileDialogMissionGoals, ID_DLG_MGOALS_BG_ROW1);
    graphic::Texture * texRow2 = tm->GetSprite(SpriteFileDialogMissionGoals, ID_DLG_MGOALS_BG_ROW2);

    const int rowH = texRow1->GetHeight();
    const int marginRowV = 5;
    const int headerGoalH = 20;
    const int marginHeaderGoalH = 20;
    const int marginGoalsGroupH = 40;
    const int marginGoals2GroupH = 35;
    const int endButtonAreaH = 65;

    const unsigned int numGoals = goals.size();
    unsigned int numPrimaryGoals = 0;
    unsigned int numPrimaryGoalsCompleted = 0;
    unsigned int numSecondaryGoals = 0;

    for(const MissionGoal & g : goals)
    {
        if(g.IsPrimary())
        {
            ++numPrimaryGoals;

            if(g.IsCompleted())
                ++numPrimaryGoalsCompleted;
        }
        else
            ++numSecondaryGoals;
    }

    if(numPrimaryGoals > 0)
        contentH += headerGoalH + marginHeaderGoalH;
    if(numSecondaryGoals > 0)
    {
        contentH += headerGoalH + marginHeaderGoalH;
        contentH += marginGoalsGroupH;
    }

    const int contentGoalsH = (numPrimaryGoals + numSecondaryGoals) * (rowH + marginRowV);
    contentH += contentGoalsH;

    contentH += endButtonAreaH;

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFileDialogMissionGoals, ID_DLG_MGOALS_BG_TOP);
    mBgTop = new graphic::Image(tex);
    RegisterRenderable(mBgTop);

    tex = tm->GetSprite(SpriteFileDialogMissionGoals, ID_DLG_MGOALS_BG_BOTTOM);
    mBgBot = new graphic::Image(tex);
    RegisterRenderable(mBgBot);

    tex = tm->GetSprite(SpriteFileDialogMissionGoalsExp, ID_DLG_MGOALS_BG_MID);
    tex->SetScaleMode(graphic::TSCALE_NEAREST);
    mBgMid = new graphic::Image(tex);
    mBgMid->SetHeight(contentH);
    RegisterRenderable(mBgMid);

    const int w = mBgTop->GetWidth();
    const int h = mBgTop->GetHeight() + mBgMid->GetHeight() + mBgBot->GetHeight();
    SetSize(w, h);

    // BUTTON CLOSE
    mBtnClose = new ButtonCloseDMG(this);

    const int buttonX = w - mBtnClose->GetWidth();
    const int buttonY = 0;
    mBtnClose->SetPosition(buttonX, buttonY);

    // -- CONTENT --
    const int marginL = 40;

    // TITLE
    const int sizeTitle = 28;
    const int marginTitleT = 14;

    auto font = fm->GetFont(WidgetsConstants::FontFileDialogTitle, sizeTitle, graphic::Font::NORMAL);
    auto title = new sgui::Label(sm->GetCString("MISSION_GOALS"), font, this);
    title->SetColor(WidgetsConstants::colorDialogTitle);
    title->SetPosition(marginL, marginTitleT);

    int contentX = marginL;
    int contentY = mBgTop->GetHeight();

    // PRIMARY GOALS
    const unsigned int colorHeader = 0xdbe9f0ff;
    const int sizeHeader = 24;

    if(numPrimaryGoals > 0)
    {
        auto font = fm->GetFont(WidgetsConstants::FontFileHeader, sizeHeader, graphic::Font::NORMAL);
        auto labelHeader = new sgui::Label(sm->GetCString("PRIMARY_GOALS"), font, this);
        labelHeader->SetColor(colorHeader);
        labelHeader->SetPosition(contentX, contentY);

        contentY += labelHeader->GetHeight() + marginHeaderGoalH;

        for(unsigned int n = 0; n < numGoals; ++n)
        {
            if(!goals[n].IsPrimary())
                continue;

            tex = 0 == (n % 2) ? texRow1 : texRow2;
            auto row = CreateGoalEntry(n, tex);
            row->SetPosition(contentX, contentY);

            contentY += row->GetHeight() + marginRowV;
        }
    }

    // SECONDARY GOALS
    if(numSecondaryGoals > 0)
    {
        contentY += marginGoalsGroupH;

        auto font = fm->GetFont(WidgetsConstants::FontFileHeader, sizeHeader, graphic::Font::NORMAL);
        auto labelHeader = new sgui::Label(sm->GetCString("SECONDARY_GOALS"), font, this);
        labelHeader->SetColor(colorHeader);
        labelHeader->SetPosition(contentX, contentY);

        contentY += labelHeader->GetHeight() + marginHeaderGoalH;

        for(unsigned int n = 0; n < numGoals; ++n)
        {
            if(goals[n].IsPrimary())
                continue;

            tex = 0 == (n % 2) ? texRow1 : texRow2;
            auto row = CreateGoalEntry(n, tex);
            row->SetPosition(contentX, contentY);

            contentY += row->GetHeight() + marginRowV;
        }

        contentY += marginGoals2GroupH;
    }

    // BUTTON END MISSION
    mBtnEnd = new ButtonEndMission(this);
    contentX = (w - mBtnEnd->GetWidth()) / 2;
    mBtnEnd->SetPosition(contentX, contentY);

    CheckIfEndAllowed();
}

unsigned int DialogMissionGoals::AddFunctionOnClose(const std::function<void()> & f)
{
    return mBtnClose->AddOnClickFunction(f);
}

void DialogMissionGoals::RemoveFunctionOnClose(unsigned int funId)
{
    mBtnClose->RemoveClickFunction(funId);
}

unsigned int DialogMissionGoals::AddFunctionOnEnd(const std::function<void()> & f)
{
    return mBtnEnd->AddOnClickFunction(f);
}

void DialogMissionGoals::RemoveFunctionOnEnd(unsigned int funId)
{
    mBtnEnd->RemoveClickFunction(funId);
}

sgl::sgui::Widget * DialogMissionGoals::CreateGoalEntry(unsigned int goalInd,
                                                        sgl::graphic::Texture * texBg)
{
    using namespace sgl;

    const std::vector<MissionGoal> & goals = mTrackerMG->GetGoals();
    const MissionGoal & g = goals[goalInd];

    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();

    const int paddingH = 20;
    const int paddingT = 10;

    int contX = paddingH;
    int contY = paddingT;

    // body
    auto bg = new sgui::Image(texBg, this);

    // CHECKBOX
    const int marginCheckboxR = 10;

    auto tm = graphic::TextureManager::Instance();
    const unsigned int texId = g.IsCompleted() ? ID_DLG_MGOALS_CHECKBOX_CHECKED :
                                                 ID_DLG_MGOALS_CHECKBOX_NORMAL;
    auto texCheckbox = tm->GetSprite(SpriteFileDialogMissionGoals, texId);

    auto checkbox = new sgui::Image(texCheckbox, bg);
    checkbox->SetPosition(contX, contY);

    contX += checkbox->GetWidth() + marginCheckboxR;

    // DESCRIPTION
    const int sizeDesc = 20;
    const unsigned int colorData = 0x8cbfd9ff;

    auto font = fm->GetFont(WidgetsConstants::FontFileText, sizeDesc, graphic::Font::NORMAL);
    auto labelDesc = new sgui::Label(g.GetDescription().c_str(), font, bg);
    labelDesc->SetColor(colorData);
    labelDesc->SetPosition(contX, contY);

    // PROGRESS
    const int marginDescV = 10;
    const int marginHeaderH = 20;
    const int marginDataH = 100;

    contY += labelDesc->GetHeight() + marginDescV;

    const int size2 = 18;
    const unsigned int colorHeader = 0xdbe9f0ff;

    auto fontHeader = fm->GetFont(WidgetsConstants::FontFileHeader, size2, graphic::Font::NORMAL);
    auto labelHeader = new sgui::Label(sm->GetCString("PROGRESS"), fontHeader, bg);
    labelHeader->SetColor(colorHeader);
    labelHeader->SetPosition(contX, contY);

    contX += labelHeader->GetWidth() + marginHeaderH;

    sgui::Label * labelData = nullptr;

    if(g.IsFailed())
        labelData = new sgui::Label("-", font, bg);
    else if(g.IsProgressUnknown())
        labelData = new sgui::Label("?", font, bg);

    if(labelData != nullptr)
    {
        labelData->SetColor(colorData);
        labelData->SetPosition(contX, contY);
    }
    else
    {
        const float min = 0.f;
        const float max = 100.f;
        auto pb = new ProgressBarObjectVisualStat(min, max, bg);
        pb->SetValue(g.GetProgress());
        pb->SetPosition(contX, contY + (labelHeader->GetHeight() - pb->GetHeight()) / 2);
    }

    // REWARD
    const int rewardX0 = 415;
    contX = rewardX0;

    labelHeader = new sgui::Label(sm->GetCString("REWARDS"), fontHeader, bg);
    labelHeader->SetColor(colorHeader);
    labelHeader->SetPosition(contX, contY);

    contX += labelHeader->GetWidth() + marginHeaderH;

    // collected or failed
    if(g.IsRewardCollected() || g.IsFailed())
    {
        auto labelData = new sgui::Label("-", font, bg);
        labelData->SetColor(colorData);
        labelData->SetPosition(contX, contY);
    }
    // not collected yet
    else
    {
        const int rewardMargin = 5;
        const int rewardMarginIcon = 30;

        const unsigned int iconIds[NUM_MISSION_REWARDS] =
        {
            ID_UIS_ICON_C_RES_BLOBS_24,
            ID_UIS_ICON_C_RES_DIAMONDS_24,
            ID_UIS_ICON_C_RES_ENERGY_24,
            ID_UIS_ICON_C_RES_MATERIAL_24,
            ID_UIS_ICON_C_RES_MONEY_24,
            ID_UIS_ICON_C_RES_RESEARCH_24,
        };

        for(unsigned int r = 0; r < NUM_MISSION_REWARDS; ++r)
        {
            const int reward = g.GetRewardByType(static_cast<MissionReward>(r));

            // no reward for this type
            if(reward <= 0)
                continue;

            std::ostringstream os;

            if(reward >= 1000)
            {
                float val = static_cast<float>(reward) / 1000.f;

                os << std::setprecision(2) << val << "K";
            }
            else
                os << reward;

            auto labelData = new sgui::Label(os.str().c_str(), fontHeader, bg);
            labelData->SetColor(colorData);
            labelData->SetPosition(contX, contY);

            contX += labelData->GetWidth() + rewardMargin;

            auto texIcon = tm->GetSprite(SpriteFileGameUIShared, iconIds[r]);
            auto icon = new sgui::Image(texIcon, bg);
            icon->SetPosition(contX, contY + ((labelData->GetHeight() - icon->GetHeight()) / 2));

            contX += icon->GetWidth() + rewardMarginIcon;
        }
    }

    // REWARD COLLECTION
    const int paddingCenterToR = 120;

    // label collected OR failed
    const int sizeCollected = 22;
    font = fm->GetFont(WidgetsConstants::FontFileStrongText, sizeDesc, graphic::Font::NORMAL);

    if(g.IsFailed())
    {
        labelData = new sgui::Label(sm->GetCString("GOAL_FAILED"), font, bg);
        labelData->SetColor(WidgetsConstants::colorDialogBad);
    }
    else
    {
        labelData = new sgui::Label(sm->GetCString("COLLECTED_REW"), font, bg);
        labelData->SetColor(WidgetsConstants::colorDialogGood);
    }

    contX = bg->GetWidth() - paddingCenterToR - (labelData->GetWidth() / 2);
    contY = (bg->GetHeight() - labelData->GetHeight()) / 2;
    labelData->SetPosition(contX, contY);

    // not collected or failed yet -> show button
    if(!g.IsRewardCollected() && !g.IsFailed())
    {
        labelData->SetVisible(false);

        auto btn = new ButtonCollect(bg);

        contX = bg->GetWidth() - paddingCenterToR - (btn->GetWidth() / 2);
        contY = (bg->GetHeight() - btn->GetHeight()) / 2;
        btn->SetPosition(contX, contY);

        btn->SetEnabled(g.IsCompleted());

        btn->AddOnClickFunction([this, btn, labelData, goalInd]
        {
            mTrackerMG->CollectMissionGoalReward(goalInd);

            btn->SetVisible(false);
            labelData->SetVisible(true);

            CheckIfEndAllowed();
        });
    }

    return bg;
}

void DialogMissionGoals::CheckIfEndAllowed()
{
    const std::vector<MissionGoal> & goals = mTrackerMG->GetGoals();

    for(const MissionGoal & g : goals)
    {
        if(g.IsPrimary() && !g.IsRewardCollected())
        {
            mBtnEnd->SetEnabled(false);
            return ;
        }
    }

    // enable END if all primary goals are completed and collected
    mBtnEnd->SetEnabled(true);
}

void DialogMissionGoals::HandlePositionChanged()
{
    SetPositions();
}

void DialogMissionGoals::SetPositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    // BACKGROUND
    int y = y0;

    mBgTop->SetPosition(x0, y);

    y += mBgTop->GetHeight();
    mBgMid->SetPosition(x0, y);

    y += mBgMid->GetHeight();
    mBgBot->SetPosition(x0, y);
}

} // namespace game
