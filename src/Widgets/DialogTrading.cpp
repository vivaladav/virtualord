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
DialogTrading::DialogTrading()
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
    auto font = fm->GetFont("Lato-Regular.ttf", 32, graphic::Font::NORMAL);

    sgui::Label * title = new sgui::Label("TRADE YOUR RESOURCES", font, this);

    const int titleX = (w - title->GetWidth()) / 2;
    const int titleY = 10;
    title->SetPosition(titleX, titleY);
    title->SetColor(0xf0f3f5ff);
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
