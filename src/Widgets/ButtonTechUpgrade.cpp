#include "Widgets/ButtonTechUpgrade.h"

#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>

namespace game
{

ButtonTechUpgrade::ButtonTechUpgrade(sgl::sgui::Widget * parent)
    : sgl::sgui::AbstractButton(parent)
    , mBg(new sgl::graphic::Image)
{
    using namespace sgl;

    SetCheckable(true);

    RegisterRenderable(mBg);

    auto tm = sgl::graphic::TextureManager::Instance();

    const unsigned int texIds[sgui::AbstractButton::NUM_VISUAL_STATES] =
    {
        ID_DLG_TECHT_BTN_UPG_NORMAL,
        ID_DLG_TECHT_BTN_UPG_DISABLED,
        ID_DLG_TECHT_BTN_UPG_OVER,
        ID_DLG_TECHT_BTN_UPG_PUSHED,
        ID_DLG_TECHT_BTN_UPG_CHECKED,
    };

    for(unsigned int i = 0; i < sgui::AbstractButton::NUM_VISUAL_STATES; ++i)
        mTexs[i] = tm->GetSprite(SpriteFileDialogTechTree, texIds[i]);

    // UPDATE CONTENT
    UpdateGraphics(NORMAL);
    UpdatePositions();
}

void ButtonTechUpgrade::HandleMouseOver()
{
    sgl::sgui::AbstractButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-03.ogg");
}

void ButtonTechUpgrade::HandleButtonDown()
{
    sgl::sgui::AbstractButton::HandleButtonDown();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_click-03.ogg");
}

void ButtonTechUpgrade::OnStateChanged(sgl::sgui::AbstractButton::VisualState state)
{
    sgl::sgui::AbstractButton::OnStateChanged(state);

    UpdateGraphics(state);
}

void ButtonTechUpgrade::UpdateGraphics(sgl::sgui::AbstractButton::VisualState state)
{
    mBg->SetTexture(mTexs[state]);

    SetSize(mBg->GetWidth(), mBg->GetHeight());
}

void ButtonTechUpgrade::HandlePositionChanged()
{
    sgl::sgui::AbstractButton::HandlePositionChanged();

    UpdatePositions();
}

void ButtonTechUpgrade::UpdatePositions()
{
    mBg->SetPosition(GetScreenX(), GetScreenY());
}

} // namespace game
