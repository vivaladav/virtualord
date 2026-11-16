#include "PanelTurnControl.h"

#include "Player.h"
#include "Widgets/GameSimpleTooltip.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/ImageButton.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringsChangeListener.h>
#include <sgl/utilities/StringManager.h>

#include <cmath>

// anonymous namespace for local "private" classes
namespace
{

using namespace game;

// ========== BUTTON GO TO BASE ==========
class ButtonGoToBase : public sgl::sgui::ImageButton, sgl::utilities::StringsChangeListener
{
public:
    ButtonGoToBase(sgl::sgui::Widget * parent)
        : sgl::sgui::ImageButton({
                                     ID_TURNC_BTN_BASE_NORMAL,
                                     ID_TURNC_BTN_BASE_DISABLED,
                                     ID_TURNC_BTN_BASE_OVER,
                                     ID_TURNC_BTN_BASE_PUSHED,
                                     ID_TURNC_BTN_BASE_NORMAL
                                 }, SpriteFilePanelTurnControl, parent)
    {
        using namespace sgl;

        auto sm = utilities::StringManager::Instance();

        // tooltip
        mTooltip = new GameSimpleTooltip(sm->GetCString("TT_BACK2BASE"));
        SetTooltip(mTooltip);
        SetTooltipDelay(WidgetsConstants::timeTooltipButtonDelay);

        // shortcut
        SetShortcutKey(core::KeyboardEvent::KEY_B, core::KeyboardEvent::MOD_SHIFT);
    }

    void OnStringsChanged() override
    {
        auto sm = sgl::utilities::StringManager::Instance();

        mTooltip->SetText(sm->GetCString("TT_BACK2BASE"));
    }

    void HandleMouseOver() override
    {
        sgl::sgui::ImageButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-01.ogg");
    }

    void HandleButtonDown() override
    {
        sgl::sgui::ImageButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-01.ogg");
    }

private:
    GameSimpleTooltip * mTooltip = nullptr;
};

// ========== BUTTON NEXT UNIT ==========
class ButtonNextUnit : public sgl::sgui::ImageButton, sgl::utilities::StringsChangeListener
{
public:
    ButtonNextUnit(sgl::sgui::Widget * parent)
        : sgl::sgui::ImageButton({
                                     ID_TURNC_BTN_NEXTU_NORMAL,
                                     ID_TURNC_BTN_NEXTU_DISABLED,
                                     ID_TURNC_BTN_NEXTU_OVER,
                                     ID_TURNC_BTN_NEXTU_PUSHED,
                                     ID_TURNC_BTN_NEXTU_NORMAL
                                 }, SpriteFilePanelTurnControl, parent)
    {
        using namespace sgl;

        auto sm = utilities::StringManager::Instance();

        // tooltip
        mTooltip = new GameSimpleTooltip(sm->GetCString("TT_NEXT_UNIT"));
        SetTooltip(mTooltip);
        SetTooltipDelay(WidgetsConstants::timeTooltipButtonDelay);

        // shortcut
        SetShortcutKey(core::KeyboardEvent::KEY_N, core::KeyboardEvent::MOD_SHIFT);
    }

    void OnStringsChanged() override
    {
        auto sm = sgl::utilities::StringManager::Instance();

        mTooltip->SetText(sm->GetCString("TT_NEXT_UNIT"));
    }

    void HandleMouseOver() override
    {
        sgl::sgui::ImageButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-01.ogg");
    }

    void HandleButtonDown() override
    {
        sgl::sgui::ImageButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-01.ogg");
    }

private:
    GameSimpleTooltip * mTooltip = nullptr;
};

// ========== BUTTON END TURN ==========
class ButtonEndTurn : public sgl::sgui::ImageButton, sgl::utilities::StringsChangeListener
{
public:
    ButtonEndTurn(sgl::sgui::Widget * parent)
        : sgl::sgui::ImageButton({
                                    ID_TURNC_BTN_NEXTT_NORMAL,
                                    ID_TURNC_BTN_NEXTT_DISABLED,
                                    ID_TURNC_BTN_NEXTT_OVER,
                                    ID_TURNC_BTN_NEXTT_PUSHED,
                                    ID_TURNC_BTN_NEXTT_NORMAL
                                 }, SpriteFilePanelTurnControl, parent)
    {
        using namespace sgl;

        auto sm = utilities::StringManager::Instance();

        // tooltip
        mTooltip = new GameSimpleTooltip(sm->GetCString("END_TURN"));
        SetTooltip(mTooltip);
        SetTooltipDelay(WidgetsConstants::timeTooltipButtonDelay);

        // shortcut
        SetShortcutKey(core::KeyboardEvent::KEY_SPACE, core::KeyboardEvent::MOD_SHIFT);
    }

    void OnStringsChanged() override
    {
        auto sm = sgl::utilities::StringManager::Instance();

        mTooltip->SetText(sm->GetCString("END_TURN"));
    }

    void HandleMouseOver() override
    {
        sgl::sgui::ImageButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-01.ogg");
    }

    void HandleButtonDown() override
    {
        sgl::sgui::ImageButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-01.ogg");
    }

private:
    GameSimpleTooltip * mTooltip = nullptr;
};

} // namespace

namespace game
{

// ========== PANEL ==========
PanelTurnControl::PanelTurnControl(Player * player, sgl::sgui::Widget * parent)
    : sgl::sgui::Widget(parent)
    , mPlayer(player)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    graphic::Texture * tex = nullptr;

    // BACKGROUND
    tex = tm->GetSprite(SpriteFilePanelTurnControl, ID_TURNC_BG);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    SetSize(w, h);

    // BUTTON GO BACK TO BASE
    mButtonBase = new ButtonGoToBase(this);

    // BUTTON NEXT UNIT
    mButtonNextUnit = new ButtonNextUnit(this);
    UpdateButtonNextUnitEnabled();

    // BUTTON END TURN
    mButtonEndTurn = new ButtonEndTurn(this);

    // TEXT
    const int fontSize = 22;
    const unsigned int colorText = 0x76a7bcff;
    graphic::Font * font = fm->GetFont(WidgetsConstants::FontFileText, fontSize, graphic::Font::NORMAL);

    mText = new sgui::Label(font, this);
    mText->SetColor(colorText);
    mText->SetVisible(false);

    // POSITION ELEMENTS
    const int marginButton = 45;
    const int spacingButton = 15;
    const int marginIcon = 5;
    const int marginBar = 5;

    int x = marginButton;
    int y = (h - mButtonBase->GetHeight()) / 2;
    mButtonBase->SetPosition(x, y);

    x += mButtonBase->GetWidth() + spacingButton;
    mButtonNextUnit->SetPosition(x, y);

    x = GetWidth() - marginButton - mButtonEndTurn->GetWidth();
    y = (h - mButtonEndTurn->GetHeight()) / 2;
    mButtonEndTurn->SetPosition(x, y);
}

void PanelTurnControl::AddFunctionGoToBase(const std::function<void()> & f)
{
    mButtonBase->AddOnClickFunction(f);
}

void  PanelTurnControl::AddFunctionNextUnit(const std::function<void()> & f)
{
    mButtonNextUnit->AddOnClickFunction(f);
}

void PanelTurnControl::AddFunctionEndTurn(const std::function<void()> & f)
{
    mButtonEndTurn->AddOnClickFunction(f);
}

void PanelTurnControl::SetButtonsEnabled(bool enabled)
{
    mButtonBase->SetEnabled(enabled);

    mButtonNextUnit->SetEnabled(enabled && mPlayer->GetNumUnits() > 0);

    mButtonEndTurn->SetEnabled(enabled);
}

void PanelTurnControl::UpdateButtonNextUnitEnabled()
{
    mButtonNextUnit->SetEnabled(mPlayer->GetNumUnits() > 0);
}

void PanelTurnControl::HideText()
{
    mButtonBase->SetVisible(true);
    mButtonEndTurn->SetVisible(true);

    mText->SetVisible(false);
}

void PanelTurnControl::ShowText(const char * text)
{
    mText->SetVisible(true);
    mText->SetText(text);

    PositionText();
}

void PanelTurnControl::HandlePositionChanged()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    mBg->SetPosition(x0, y0);
}

void PanelTurnControl::PositionText()
{
    const int textAreaX0 = 203;
    const int textAreaY0 = 16;
    const int textAreaW = 500;
    const int textAreaH = 32;
    const int padding = 20;

    const int x = textAreaX0 + padding;
    const int y = textAreaY0 + (textAreaH - mText->GetHeight()) / 2;

    mText->SetPosition(x, y);
}

} // namespace game
