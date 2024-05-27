#include "Widgets/DialogTrading.h"

#include "Game.h"
#include "Widgets/GameButton.h"
#include "Widgets/GameUIData.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/ImageButton.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/System.h>

#include <sstream>

namespace game
{

// ===== BUTTON =====
class ButtonDialogTradingClose : public sgl::sgui::ImageButton
{
public:
    ButtonDialogTradingClose(sgl::sgui::Widget * parent)
        : sgl::sgui::ImageButton({ ID_DLG_TRADING_BTN_CLOSE_NORMAL, ID_DLG_TRADING_BTN_CLOSE_NORMAL,
                                   ID_DLG_TRADING_BTN_CLOSE_OVER, ID_DLG_TRADING_BTN_CLOSE_PUSHED,
                                   ID_DLG_TRADING_BTN_CLOSE_PUSHED }, SpriteFileDialogTrading, parent)
    {
        SetShortcutKey(sgl::core::KeyboardEvent::KEY_ESCAPE);
    }

    void HandleMouseOver() override
    {
        sgl::sgui::AbstractButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-01.ogg");
    }

    void HandleButtonDown() override
    {
        sgl::sgui::AbstractButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_click-01.ogg");
    }
};

// ===== DIALOG =====
DialogTrading::DialogTrading(Player * p)
    : mPlayer(p)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_BG);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    SetSize(w, h);

    // BUTTON CLOSE
    mButtonClose = new ButtonDialogTradingClose(this);

    const int buttonX = w - mButtonClose->GetWidth();
    mButtonClose->SetX(buttonX);

    // TITLE
    auto fontTitle = fm->GetFont("Lato-Regular.ttf", 32, graphic::Font::NORMAL);

    sgui::Label * title = new sgui::Label("TRADE YOUR RESOURCES", fontTitle, this);

    const int titleX = (w - title->GetWidth()) / 2;
    const int titleY = 5;
    title->SetPosition(titleX, titleY);
    title->SetColor(0xf0f3f5ff);

    // -- HEADERS --
    const int hbX0 = 40;
    const int hbY0 = 75;
    const int hbH = 40;
    const int hbW1 = 300;
    const int hbW2 = 200;
    const int hbW3 = 350;
    const int hbW4 = 350;
    const int marginHB = 20;

    const unsigned int colorHeader = 0xd6eaf5ff;
    auto fontHeader = fm->GetFont("Lato-Regular.ttf", 22, graphic::Font::NORMAL);

    // RESOURCE
    sgui::Label * header = new sgui::Label("RESOURCE", fontHeader, this);

    int hbX = hbX0;
    int headerX = hbX + (hbW1 - header->GetWidth()) / 2;
    int headerY = hbY0 + (hbH - header->GetHeight()) / 2;
    header->SetPosition(headerX, headerY);
    header->SetColor(colorHeader);

    hbX += hbW1 + marginHB;

    // STOCK
    header = new sgui::Label("STOCK", fontHeader, this);

    headerX = hbX + (hbW2 - header->GetWidth()) / 2;
    headerY = hbY0 + (hbH - header->GetHeight()) / 2;
    header->SetPosition(headerX, headerY);
    header->SetColor(colorHeader);

    hbX += hbW2 + marginHB;

    // BUY
    header = new sgui::Label("BUY", fontHeader, this);

    headerX = hbX + (hbW3 - header->GetWidth()) / 2;
    headerY = hbY0 + (hbH - header->GetHeight()) / 2;
    header->SetPosition(headerX, headerY);
    header->SetColor(colorHeader);

    hbX += hbW3 + marginHB;

    // SELL
    header = new sgui::Label("SELL", fontHeader, this);

    headerX = hbX + (hbW4 - header->GetWidth()) / 2;
    headerY = hbY0 + (hbH - header->GetHeight()) / 2;
    header->SetPosition(headerX, headerY);
    header->SetColor(colorHeader);

    // -- LABELS STOCK --

}

void DialogTrading::SetFunctionOnClose(const std::function<void()> & f)
{
    mButtonClose->AddOnClickFunction(f);
}

void DialogTrading::HandlePositionChanged()
{
    SetPositions();
}

void DialogTrading::SetPositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    // BACKGROUND
    mBg->SetPosition(x0, y0);
}

} // namespace game
