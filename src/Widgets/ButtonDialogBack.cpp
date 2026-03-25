#include "ButtonDialogBack.h"

#include "Widgets/GameUIData.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>

namespace game
{

ButtonDialogBack::ButtonDialogBack(sgl::sgui::Widget * parent)
    : sgl::sgui::ImageButton({ ID_BTN_DLG_BACK_NORMAL, ID_BTN_DLG_BACK_DISABLED,
                               ID_BTN_DLG_BACK_OVER, ID_BTN_DLG_BACK_PUSHED,
                               ID_BTN_DLG_BACK_PUSHED }, SpriteFileUIShared, parent)
{
    SetShortcutKey(sgl::core::KeyboardEvent::KEY_ESCAPE);
}

void ButtonDialogBack::HandleMouseOver()
{
    sgl::sgui::AbstractButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-02.ogg");
}

void ButtonDialogBack::HandleButtonDown()
{
    sgl::sgui::AbstractButton::HandleButtonDown();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_click-02.ogg");
}

} // namespace game
