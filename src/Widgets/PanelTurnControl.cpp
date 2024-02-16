#include "PanelTurnControl.h"

#include "Player.h"
#include "Widgets/DigitsDisplay.h"
#include "Widgets/GameSimpleTooltip.h"
#include "Widgets/GameUIData.h"
#include "Widgets/ProgressBarTurnEnergy.h"

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

#include <cmath>

namespace game
{

// ========== BUTTON END TURN ==========
class ButtonEndTurn : public sgl::sgui::ImageButton
{
public:
    ButtonEndTurn(sgl::sgui::Widget * parent)
        : sgl::sgui::ImageButton({
                                     ID_TURN_CONTROL_BUTTON_NORMAL,
                                     ID_TURN_CONTROL_BUTTON_DISABLED,
                                     ID_TURN_CONTROL_BUTTON_OVER,
                                     ID_TURN_CONTROL_BUTTON_PUSHED,
                                     ID_TURN_CONTROL_BUTTON_NORMAL
                                 }, SpriteFilePanelTurnControl, parent)
    {
        // tooltip
        const int ttDelay = 500;
        auto tt = new GameSimpleTooltip("End your turn");
        SetTooltip(tt);
        SetTooltipDelay(ttDelay);

        // shortcut
        SetShortcutKey(sgl::core::KeyboardEvent::KEY_BACKSPACE);
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
};

// ========== PANEL ==========
PanelTurnControl::PanelTurnControl(Player * player, sgl::sgui::Widget * parent)
    : sgl::sgui::Widget(parent)
    , mPlayer(player)
{
    using namespace sgl;

    const PlayerFaction faction = player->GetFaction();
    const float minEnergy = 0.f;
    const float maxEnergy = player->GetTurnMaxEnergy();
    const float energy = player->GetTurnEnergy();

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    graphic::Texture * tex = nullptr;

    // BACKGROUND
    tex = tm->GetSprite(SpriteFilePanelTurnControl, ID_TURN_CONTROL_BG);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    SetSize(w, h);

    // ICON ENERGY
    const unsigned int iconId = ID_TURN_CONTROL_ICON_F1 + faction;
    tex = tm->GetSprite(SpriteFilePanelTurnControl, iconId);
    mIconEnergy = new sgui::Image(tex, this);

    // PROGRESS BAR ENERGY
    mEnergyBar = new ProgressBarTurnEnergy(faction, minEnergy, maxEnergy, this);
    mEnergyBar->SetValue(energy);

    // DIGITS DISPLAY
    const unsigned int colorDigits = 0x70a9c2ff;
    const unsigned int colorZeros = 0x35677dff;
    const int fontDigitsSize = 20;
    const int digits = 4;
    mDigits = new DigitsDisplay(digits, fontDigitsSize, std::string(), this);
    mDigits->SetColorDigits(colorDigits);
    mDigits->SetColorZeros(colorZeros);
    mDigits->SetValue(energy);

    // BUTTON END TURN
    mButtonEndTurn = new ButtonEndTurn(this);

    // TEXT
    const char * fontFile = "Lato-Regular.ttf";
    const int fontSize = 28;
    const unsigned int colorText = 0x76a7bcff;
    graphic::Font * font = fm->GetFont(fontFile, fontSize, graphic::Font::NORMAL);

    mText = new sgui::Label(font, this);
    mText->SetColor(colorText);
    mText->SetVisible(false);

    // CALLBACK ENERGY
    player->SetOnTurnEnergyChanged([this]
    {
        const int energy = std::roundf(mPlayer->GetTurnEnergy());
        mEnergyBar->SetValue(energy);

        mDigits->SetValue(energy);
    });

    player->SetOnTurnMaxEnergyChanged([this]
    {
        const float minEnergy = 0.f;
        const float maxEnergy = mPlayer->GetTurnMaxEnergy();
        mEnergyBar->SetMinMax(minEnergy, maxEnergy);
    });

    // POSITION ELEMENTS
    const int marginIcon = 5;
    const int marginBar = 10;
    const int marginDigits = 30;
    const int totW = mIconEnergy->GetWidth() + marginIcon + mEnergyBar->GetWidth() +
                     marginBar + mDigits->GetWidth() + marginDigits + mButtonEndTurn->GetWidth();
    const int x0 = (w - totW) / 2;

    int x = x0;
    int y = (h - mIconEnergy->GetHeight()) / 2;
    mIconEnergy->SetPosition(x, y);

    x += mIconEnergy->GetWidth() + marginIcon;
    y = (h - mEnergyBar->GetHeight()) / 2;
    mEnergyBar->SetPosition(x, y);

    x += mEnergyBar->GetWidth() + marginBar;
    y = (h - mDigits->GetHeight()) / 2;
    mDigits->SetPosition(x, y);

    x += mDigits->GetWidth() + marginDigits;
    y = (h - mButtonEndTurn->GetHeight()) / 2;
    mButtonEndTurn->SetPosition(x, y);
}

PanelTurnControl::~PanelTurnControl()
{
    mPlayer->SetOnTurnEnergyChanged([]{});
    mPlayer->SetOnTurnMaxEnergyChanged([]{});
}

void PanelTurnControl::SetFunctionEndTurn(const std::function<void()> & f)
{
    mButtonEndTurn->AddOnClickFunction(f);
}

void PanelTurnControl::SetButtonEndTurnEnabled(bool enabled)
{
    mButtonEndTurn->SetEnabled(enabled);
}

void PanelTurnControl::ShowPanel()
{
    mIconEnergy->SetVisible(true);
    mEnergyBar->SetVisible(true);
    mDigits->SetVisible(true);
    mButtonEndTurn->SetVisible(true);

    mText->SetVisible(false);
}

void PanelTurnControl::ShowText(const char * text)
{
    mIconEnergy->SetVisible(false);
    mEnergyBar->SetVisible(false);
    mDigits->SetVisible(false);
    mButtonEndTurn->SetVisible(false);

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
    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    const int x = (w - mText->GetWidth()) / 2;
    const int y = (h - mText->GetHeight()) / 2;

    mText->SetPosition(x, y);
}

} // namespace game
