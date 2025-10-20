#include "Widgets/DialogMissionGoals.h"

#include "Game.h"
#include "Widgets/GameUIData.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
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
#include <sstream>

namespace game
{

// ====== BUTTON CLOSE =====
class ButtonClose : public sgl::sgui::ImageButton
{
public:
    ButtonClose(sgl::sgui::Widget * parent)
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

// ====== BUTTON CLOSE =====
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
        const int sizeTitle = 18;

        auto font = fm->GetFont(fileFont, sizeTitle, graphic::Font::NORMAL);
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

// ===== DIALOG =====
DialogMissionGoals::DialogMissionGoals(const std::vector<MissionGoal> & goals)
    : mGoals(goals)
{
    using namespace sgl;

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

    const unsigned int numGoals = mGoals.size();
    unsigned int numPrimaryGoals = 0;
    unsigned int numPrimaryGoalsCompleted = 0;
    unsigned int numSecondaryGoals = 0;

    for(const MissionGoal & g : mGoals)
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
    tex->SetScaleMode(0);
    mBgMid = new graphic::Image(tex);
    mBgMid->SetHeight(contentH);
    RegisterRenderable(mBgMid);

    const int w = mBgTop->GetWidth();
    const int h = mBgTop->GetHeight() + mBgMid->GetHeight() + mBgBot->GetHeight();
    SetSize(w, h);

    // BUTTON CLOSE
    mBtnClose = new ButtonClose(this);

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
    mTitle = new sgui::Label("MISSION GOALS", font, this);
    mTitle->SetColor(colorTitle);

    const int titleX = (w - mTitle->GetWidth()) / 2;
    mTitle->SetPosition(titleX, marginTitleT);

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
            if(!mGoals[n].IsPrimary())
                continue;

            tex = 0 == (n % 2) ? texRow1 : texRow2;
            auto row = CreateGoalEntry(n, tex);
            row->SetPosition(contentX, contentY);

            contentY += row->GetHeight() + marginRowV;
        }
    }

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
            if(mGoals[n].IsPrimary())
                continue;

            tex = 0 == (n % 2) ? texRow1 : texRow2;
            auto row = CreateGoalEntry(n, tex);
            row->SetPosition(contentX, contentY);

            contentY += row->GetHeight() + marginRowV;
        }

        contentY += marginGoals2GroupH;
    }

    mBtnEnd = new ButtonEndMission(this);
    contentX = (w - mBtnEnd->GetWidth()) / 2;
    mBtnEnd->SetPosition(contentX, contentY);

    mBtnEnd->SetEnabled(numPrimaryGoalsCompleted == numPrimaryGoals);
}

void DialogMissionGoals::SetFunctionOnClose(const std::function<void()> & f)
{
    mBtnClose->AddOnClickFunction(f);
}

sgl::sgui::Widget * DialogMissionGoals::CreateGoalEntry(unsigned int goalInd,
                                                        sgl::graphic::Texture * texBg)
{
    using namespace sgl;

    const MissionGoal & g = mGoals[goalInd];

    auto fm = graphic::FontManager::Instance();
    const char * fileFont = "Lato-Regular.ttf";

    const int paddingL = 20;
    const int paddingT = 10;

    int contX = paddingL;
    int contY = paddingT;

    // body
    auto bg = new sgui::Image(texBg, this);

    // checkbox
    const int marginCheckboxR = 10;

    auto tm = graphic::TextureManager::Instance();
    const unsigned int texId = g.IsCompleted() ? ID_DLG_MGOALS_CHECKBOX_CHECKED :
                                                 ID_DLG_MGOALS_CHECKBOX_NORMAL;
    auto texCheckbox = tm->GetSprite(SpriteFileDialogMissionGoals, texId);

    auto checkbox = new sgui::Image(texCheckbox, bg);
    checkbox->SetPosition(contX, contY);

    contX += checkbox->GetWidth() + marginCheckboxR;

    // description
    const int sizeDesc = 20;
    const unsigned int colorDesc = 0x8cbfd9ff;

    auto font = fm->GetFont(fileFont, sizeDesc, graphic::Font::NORMAL);
    auto labelDesc = new sgui::Label(g.GetDescription().c_str(), font, bg);
    labelDesc->SetColor(colorDesc);
    labelDesc->SetPosition(contX, contY);

    return bg;
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
