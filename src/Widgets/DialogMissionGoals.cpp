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

// ===== DIALOG =====
DialogMissionGoals::DialogMissionGoals()
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();

    // TODO define based on content
    const int contentH = 100;

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
    const unsigned int colorTitle = 0xf1f3f4ff;
    const int marginTitleT = 14;

    auto font = fm->GetFont("Lato-Regular.ttf", 28, sgl::graphic::Font::NORMAL);
    mTitle = new sgui::Label("MISSION GOALS", font, this);
    mTitle->SetColor(colorTitle);

    const int titleX = (w - mTitle->GetWidth()) / 2;
    mTitle->SetPosition(titleX, marginTitleT);
}

void DialogMissionGoals::SetFunctionOnClose(const std::function<void()> & f)
{
    mBtnClose->AddOnClickFunction(f);
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
