#include "Widgets/ButtonDialogClose.h"

#include "Widgets/GameUIData.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>

namespace game
{

ButtonDialogClose::ButtonDialogClose(sgl::sgui::Widget * parent)
    : sgl::sgui::ImageButton({ ID_BTN_DLG_CLOSE_NORMAL, ID_BTN_DLG_CLOSE_DISABLED,
                               ID_BTN_DLG_CLOSE_OVER, ID_BTN_DLG_CLOSE_PUSHED,
                               ID_BTN_DLG_CLOSE_PUSHED }, SpriteFileUIShared, parent)
{
    SetShortcutKey(sgl::core::KeyboardEvent::KEY_ESCAPE);
}

void ButtonDialogClose::HandleMouseOver()
{
    sgl::sgui::AbstractButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-02.ogg");
}

void ButtonDialogClose::HandleButtonDown()
{
    sgl::sgui::AbstractButton::HandleButtonDown();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_click-02.ogg");
}

} // namespace game
