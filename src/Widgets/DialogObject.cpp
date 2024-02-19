#include "Widgets/DialogObject.h"

#include "Game.h"
#include "GameObjects/GameObject.h"
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

// ====== BUTTON CLOSE =====
class ButtonClose : public sgl::sgui::ImageButton
{
public:
    ButtonClose(sgl::sgui::Widget * parent)
        : sgl::sgui::ImageButton({
                                    ID_DLG_OBJ_BTN_CLOSE_NORMAL,
                                    ID_DLG_OBJ_BTN_CLOSE_NORMAL,
                                    ID_DLG_OBJ_BTN_CLOSE_OVER,
                                    ID_DLG_OBJ_BTN_CLOSE_PUSHED,
                                    ID_DLG_OBJ_BTN_CLOSE_NORMAL
                                 },
                                 SpriteFileDialogObject, parent)
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
DialogObject::DialogObject()
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFileDialogObject, ID_DLG_OBJ_BG);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    SetSize(w, h);

    // BUTTON CLOSE
    mBtnClose = new ButtonClose(this);

    const int buttonX = w - mBtnClose->GetWidth();
    const int buttonY = 0;
    mBtnClose->SetPosition(buttonX, buttonY);

    // -- CONTENT --
    const int marginL = 30;

    // TITLE
    const unsigned int colorTitle = 0xf0f3f5ff;
    const int marginTitleT = 14;

    auto font = fm->GetFont("Lato-Regular.ttf", 28, sgl::graphic::Font::NORMAL);
    mTitle = new sgui::Label(font, this);
    mTitle->SetColor(colorTitle);
    mTitle->SetPosition(marginL, marginTitleT);
}

void DialogObject::SetFunctionOnClose(const std::function<void()> & f)
{
    mBtnClose->AddOnClickFunction(f);
}

void DialogObject::SetObject(GameObject * obj)
{
    const GameObjectTypeId type = obj->GetObjectType();

    // TITLE
    mTitle->SetText(GameObject::TITLES.at(type).c_str());
}

void DialogObject::HandlePositionChanged()
{
    SetPositions();
}

void DialogObject::SetPositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    // BACKGROUND
    mBg->SetPosition(x0, y0);
}

} // namespace game
