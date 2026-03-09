#include "Widgets/DialogMissionGoals.h"

#include "GameConstants.h"
#include "MissionGoal.h"
#include "MissionGoalsTracker.h"
#include "Widgets/ButtonDialogClose.h"
#include "Widgets/GameButton.h"
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

constexpr int rowW = 1200;
constexpr int rowH = 80;

// ====== BUTTON END =====
class ButtonEndMission : public GameButton
{
public:
    ButtonEndMission(sgl::sgui::Widget * parent)
        : GameButton(SpriteFileDialogMissionGoals,
                     { ID_DLG_MGOALS_BTN_END_NORMAL, ID_DLG_MGOALS_BTN_END_DISABLED,
                       ID_DLG_MGOALS_BTN_END_OVER, ID_DLG_MGOALS_BTN_END_PUSHED,
                       ID_DLG_MGOALS_BTN_END_NORMAL },
                     { 0xd9f2e1ff, 0x4d7f5eff, 0xecf9f0ff, 0xb3e5c3ff, 0xd9f2e1ff }, parent)
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
                     { 0xd6eaf5ff, 0x526c7aff, 0xebf4f9ff, 0xeaf5faff, 0xb3d3e5ff }, parent)
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
    auto texRow1 = tm->GetSprite(SpriteFileDialogMissionGoalsExp, ID_DLG_MGOALS_BG_ROW1);
    texRow1->SetScaleMode(graphic::TSCALE_NEAREST);
    auto texRow2 = tm->GetSprite(SpriteFileDialogMissionGoalsExp, ID_DLG_MGOALS_BG_ROW2);
    texRow1->SetScaleMode(graphic::TSCALE_NEAREST);
    auto texHeaderBar = tm->GetSprite(SpriteFileDialogMissionGoalsExp, ID_DLG_MGOALS_SECTION_LINE);
    texHeaderBar->SetScaleMode(graphic::TSCALE_NEAREST);

    const int marginRowV = 5;
    const int headerGoalH = 20;
    const int marginHeaderGoalH = 25;
    const int marginGoalsGroupH = 40;
    const int marginGoals2GroupH = 35;
    const int paddingHeaderGoalH = 5;
    const int endButtonAreaH = 45;

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

    int contentH = WidgetsConstants::MarginDialogContentT;

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
    mBtnClose = new ButtonDialogClose(this);

    const int buttonX = w - mBtnClose->GetWidth();
    const int buttonY = 0;
    mBtnClose->SetPosition(buttonX, buttonY);

    // -- CONTENT --
    // TITLE
    auto font = fm->GetFont(WidgetsConstants::FontFileDialogTitle,
                            WidgetsConstants::FontSizeDialogTitle, graphic::Font::NORMAL);
    auto title = new sgui::Label(sm->GetCString("MISSION_GOALS"), font, this);
    title->SetColor(WidgetsConstants::colorDialogTitle);

    const int titleY = (WidgetsConstants::DialogTitleBarH - title->GetHeight()) / 2;
    title->SetPosition(WidgetsConstants::MarginDialogTitleL, titleY);

    int contentX = WidgetsConstants::MarginDialogContentL;
    int contentY = WidgetsConstants::DialogTitleBarH + WidgetsConstants::MarginDialogContentT;

    // PRIMARY GOALS
    const unsigned int colorHeader = 0xdbe9f0ff;
    const int sizeHeader = 24;

    if(numPrimaryGoals > 0)
    {
        auto font = fm->GetFont(WidgetsConstants::FontFileHeader, sizeHeader, graphic::Font::NORMAL);
        auto labelHeader = new sgui::Label(sm->GetCString("PRIMARY_GOALS"), font, this);
        labelHeader->SetColor(colorHeader);
        labelHeader->SetPosition(contentX, contentY);

        auto headerBar = new sgui::Image(texHeaderBar, this);
        headerBar->SetImageWidth(rowW);
        headerBar->SetPosition(contentX, contentY + labelHeader->GetHeight() + paddingHeaderGoalH);

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

        auto headerBar = new sgui::Image(texHeaderBar, this);
        headerBar->SetImageWidth(rowW);
        headerBar->SetPosition(contentX, contentY + labelHeader->GetHeight() + paddingHeaderGoalH);

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
    bg->SetImageSize(rowW, rowH);

    // CHECKBOX
    const int marginCheckboxR = 10;

    auto tm = graphic::TextureManager::Instance();
    const unsigned int texId = g.IsCompleted() ? ID_CHECKBOX_CHECKED :
                                                 ID_CHECKBOX_NORMAL;
    auto texCheckbox = tm->GetSprite(SpriteFileGameUIShared, texId);

    auto checkbox = new sgui::Image(texCheckbox, bg);
    checkbox->SetPosition(contX, contY);

    contX += checkbox->GetWidth() + marginCheckboxR;

    // DESCRIPTION
    const int sizeDesc = 20;
    const unsigned int colorData = 0x8cbfd9ff;

    const std::string desc = g.IsHidden() ? sm->GetString("HIDDEN_GOAL") :
                             g.GetDescription();

    auto fontDesc = fm->GetFont(WidgetsConstants::FontFileText, sizeDesc, graphic::Font::NORMAL);
    auto labelDesc = new sgui::Label(desc.c_str(), fontDesc, bg);
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

    auto fontData = fm->GetFont(WidgetsConstants::FontFileText, size2, graphic::Font::NORMAL);
    sgui::Label * labelData = nullptr;

    if(g.IsFailed())
        labelData = new sgui::Label("-", fontData, bg);
    else if(g.IsProgressUnknown() || g.IsHidden())
        labelData = new sgui::Label("?", fontData, bg);

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
    if(g.IsRewardCollected() || g.IsFailed() || g.IsHidden())
    {
        const char * textReward = g.IsHidden() ? "?" : "-";

        auto labelData = new sgui::Label(textReward, fontData, bg);
        labelData->SetColor(colorData);
        labelData->SetPosition(contX, contY);
    }
    // not collected yet
    else
    {
        const int rewardMargin = 5;
        const int rewardMarginIcon = 30;

        const unsigned int iconIds[NUM_EXTENDED_RESOURCES] =
        {
            ID_UIS_ICON_C_RES_ENERGY_24,
            ID_UIS_ICON_C_RES_MATERIAL_24,
            ID_UIS_ICON_C_RES_DIAMONDS_24,
            ID_UIS_ICON_C_RES_BLOBS_24,
            ID_UIS_ICON_C_RES_MONEY_24,
            ID_UIS_ICON_C_RES_RESEARCH_24,
        };

        for(unsigned int r = 0; r < NUM_EXTENDED_RESOURCES; ++r)
        {
            const int reward = g.GetRewardByType(static_cast<ExtendedResource>(r));

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
    auto fontOutcome = fm->GetFont(WidgetsConstants::FontFileStrongText, sizeDesc,
                                   graphic::Font::NORMAL);

    if(g.IsFailed())
    {
        labelData = new sgui::Label(sm->GetCString("GOAL_FAILED"), fontOutcome, bg);
        labelData->SetColor(WidgetsConstants::colorDialogBad);
    }
    else if(g.GetExtraValue() > 0 && !g.IsCompleted())
    {
        const MissionGoalType goalType = g.GetType();

        std::ostringstream ss;

        if(goalType == MissionGoal::TYPE_TERRITORY_CONTROL_TIME)
        {
            const unsigned int secsInMin = 60;
            const unsigned int timeLimit = g.GetExtraValue() * secsInMin;
            const unsigned int timePlayed = mTrackerMG->GetPlayedTime();
            const unsigned int timeLeft = timeLimit - timePlayed;

            const unsigned int minutes = timeLeft / secsInMin;
            const unsigned int seconds = timeLeft - (minutes * secsInMin);
            ss << std::setw(2) << std::setfill('0') << minutes << ":" << std::setw(2) <<  seconds;
        }
        else if(goalType == MissionGoal::TYPE_TERRITORY_CONTROL_TURNS)
        {
            const unsigned int turnsLimit = g.GetExtraValue();
            const unsigned int turnsPlayed = mTrackerMG->GetPlayedTurns();
            const unsigned int turnsLeft = turnsLimit - turnsPlayed;

            ss << turnsLeft;
        }

        labelData = new sgui::Label(ss.str().c_str(), fontOutcome, bg);
        labelData->SetColor(WidgetsConstants::colorDialogText);
    }
    // leave COLLECTED label as default
    else
    {
        labelData = new sgui::Label(sm->GetCString("COLLECTED_REW"), fontOutcome, bg);
        labelData->SetColor(WidgetsConstants::colorDialogGood);
    }

    contX = bg->GetWidth() - paddingCenterToR - (labelData->GetWidth() / 2);
    contY = (bg->GetHeight() - labelData->GetHeight()) / 2;
    labelData->SetPosition(contX, contY);

    // not collected or failed yet and no extra value -> show button
    if(!g.IsRewardCollected() && !g.IsFailed() && !(g.GetExtraValue() > 0 && !g.IsCompleted()))
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
