#include "Widgets/DialogMissionGoals.h"

#include "MissionGoal.h"
#include "Screens/ScreenGame.h"
#include "Widgets/GameUIData.h"
#include "Widgets/ProgressBarObjectVisualStat.h"

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

#include <cmath>
#include <iomanip>
#include <sstream>

namespace game
{

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
class ButtonEndMission : public sgl::sgui::ImageButton
{
public:
    ButtonEndMission(sgl::sgui::Widget * parent)
        : sgl::sgui::ImageButton({
                                     ID_DLG_MGOALS_BTN_END_NORMAL,
                                     ID_DLG_MGOALS_BTN_END_DISABLED,
                                     ID_DLG_MGOALS_BTN_END_OVER,
                                     ID_DLG_MGOALS_BTN_END_PUSHED,
                                     ID_DLG_MGOALS_BTN_END_NORMAL
                                 },
                                 SpriteFileDialogMissionGoals, parent)
    {
        using namespace sgl;

        auto fm = graphic::FontManager::Instance();
        const char * fileFont = "Lato-Regular.ttf";
        const int size = 18;

        auto font = fm->GetFont(fileFont, size, graphic::Font::NORMAL);
        SetLabelFont(font);

        SetLabel("END MISSION");
    }

private:
    void OnStateChanged(sgl::sgui::AbstractButton::VisualState state) override
    {
        sgl::sgui::ImageButton::OnStateChanged(state);

        const unsigned int colorsLabel[NUM_VISUAL_STATES] =
        {
            0xd7f4deff,
            0x436f4dff,
            0xebf9eeff,
            0xc3eeceff,
            0xc3eeceff
        };

        SetLabelColor(colorsLabel[state]);
    }

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

// ====== BUTTON COLLECT =====
class ButtonCollect : public sgl::sgui::ImageButton
{
public:
    ButtonCollect(sgl::sgui::Widget * parent)
        : sgl::sgui::ImageButton({
                                     ID_DLG_MGOALS_BTN_COLLECT_NORMAL,
                                     ID_DLG_MGOALS_BTN_COLLECT_DISABLED,
                                     ID_DLG_MGOALS_BTN_COLLECT_OVER,
                                     ID_DLG_MGOALS_BTN_COLLECT_PUSHED,
                                     ID_DLG_MGOALS_BTN_COLLECT_NORMAL
                                 },
                                 SpriteFileDialogMissionGoals, parent)
    {
        using namespace sgl;

        auto fm = graphic::FontManager::Instance();
        const char * fileFont = "Lato-Regular.ttf";
        const int size = 18;

        auto font = fm->GetFont(fileFont, size, graphic::Font::NORMAL);
        SetLabelFont(font);

        SetLabel("COLLECT");
    }

private:
    void OnStateChanged(sgl::sgui::AbstractButton::VisualState state) override
    {
        sgl::sgui::ImageButton::OnStateChanged(state);

        const unsigned int colorsLabel[NUM_VISUAL_STATES] =
            {
                0xc3dfeeff,
                0x4d6673ff,
                0xebf4f9ff,
                0xc3dfeeff,
                0xc3dfeeff
            };

        SetLabelColor(colorsLabel[state]);
    }

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

// ===== DIALOG =====
DialogMissionGoals::DialogMissionGoals(ScreenGame * screen)
    : mScreen(screen)
{
    using namespace sgl;

    const std::vector<MissionGoal> & goals = mScreen->GetMissionGoals();

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();

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
    const char * fileFont = "Lato-Regular.ttf";
    const unsigned int colorTitle = 0xf1f3f4ff;
    const int sizeTitle = 28;
    const int marginTitleT = 14;

    auto font = fm->GetFont(fileFont, sizeTitle, graphic::Font::NORMAL);
    auto title = new sgui::Label("MISSION GOALS", font, this);
    title->SetColor(colorTitle);

    const int titleX = (w - title->GetWidth()) / 2;
    title->SetPosition(titleX, marginTitleT);

    int contentX = marginL;
    int contentY = mBgTop->GetHeight();

    // PRIMARY GOALS
    const unsigned int colorHeader = 0xdbe9f0ff;
    const int sizeHeader = 24;

    if(numPrimaryGoals > 0)
    {
        auto font = fm->GetFont(fileFont, sizeHeader, graphic::Font::NORMAL);
        auto labelHeader = new sgui::Label("PRIMARY GOALS", font, this);
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

        auto font = fm->GetFont(fileFont, sizeHeader, graphic::Font::NORMAL);
        auto labelHeader = new sgui::Label("SECONDARY GOALS", font, this);
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

    const std::vector<MissionGoal> & goals = mScreen->GetMissionGoals();
    const MissionGoal & g = goals[goalInd];

    auto fm = graphic::FontManager::Instance();
    const char * fileFont = "Lato-Regular.ttf";

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

    auto font = fm->GetFont(fileFont, sizeDesc, graphic::Font::NORMAL);
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

    auto font2 = fm->GetFont(fileFont, size2, graphic::Font::NORMAL);
    auto labelHeader = new sgui::Label("PROGRESS", font2, bg);
    labelHeader->SetColor(colorHeader);
    labelHeader->SetPosition(contX, contY);

    contX += labelHeader->GetWidth() + marginHeaderH;

    if(g.IsProgressUnknown())
    {
        auto labelData = new sgui::Label("?", font2, bg);
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

    labelHeader = new sgui::Label("REWARD", font2, bg);
    labelHeader->SetColor(colorHeader);
    labelHeader->SetPosition(contX, contY);

    contX += labelHeader->GetWidth() + marginHeaderH;

    // collected
    if(g.IsRewardCollected())
    {
        auto labelData = new sgui::Label("-", font2, bg);
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
            ID_DLG_MGOALS_ICON_BLOBS,
            ID_DLG_MGOALS_ICON_DIAMONDS,
            ID_DLG_MGOALS_ICON_ENERGY,
            ID_DLG_MGOALS_ICON_MATERIAL,
            ID_DLG_MGOALS_ICON_MONEY,
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

            auto labelData = new sgui::Label(os.str().c_str(), font2, bg);
            labelData->SetColor(colorData);
            labelData->SetPosition(contX, contY);

            contX += labelData->GetWidth() + rewardMargin;

            auto texIcon = tm->GetSprite(SpriteFileDialogMissionGoals, iconIds[r]);
            auto icon = new sgui::Image(texIcon, bg);
            icon->SetPosition(contX, contY + ((labelData->GetHeight() - icon->GetHeight()) / 2));

            contX += icon->GetWidth() + rewardMarginIcon;
        }
    }

    // REWARD COLLECTION
    const int paddingCenterToR = 120;

    // label collected
    const unsigned int colorCollected = 0x67e486ff;
    const int sizeCollected = 22;
    font = fm->GetFont(fileFont, sizeDesc, graphic::Font::NORMAL);

    auto labelData = new sgui::Label("COLLECTED", font, bg);
    labelData->SetColor(colorCollected);

    contX = bg->GetWidth() - paddingCenterToR - (labelData->GetWidth() / 2);
    contY = (bg->GetHeight() - labelData->GetHeight()) / 2;
    labelData->SetPosition(contX, contY);

    labelData->SetVisible(g.IsRewardCollected());

    // not collected yet -> button
    if(!g.IsRewardCollected())
    {
        auto btn = new ButtonCollect(bg);

        contX = bg->GetWidth() - paddingCenterToR - (btn->GetWidth() / 2);
        contY = (bg->GetHeight() - btn->GetHeight()) / 2;
        btn->SetPosition(contX, contY);

        btn->SetEnabled(g.IsCompleted());

        btn->AddOnClickFunction([this, btn, labelData, goalInd]
        {
            mScreen->CollectMissionGoalReward(goalInd);

            btn->SetVisible(false);
            labelData->SetVisible(true);

            CheckIfEndAllowed();
        });
    }

    return bg;
}

void DialogMissionGoals::CheckIfEndAllowed()
{
    const std::vector<MissionGoal> & goals = mScreen->GetMissionGoals();

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
