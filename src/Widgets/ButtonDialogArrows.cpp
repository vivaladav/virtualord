#include "Widgets/ButtonDialogArrows.h"

#include "Widgets/GameUIData.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>

namespace game
{

// ===== BUTTON LEFT ARROW =====
ButtonDialogArrowLeft::ButtonDialogArrowLeft(sgl::sgui::Widget * parent)
    : sgl::sgui::ImageButton({ ID_DLG_NEWEL_LEFT_NORMAL, ID_DLG_NEWEL_LEFT_DISABLED,
                               ID_DLG_NEWEL_LEFT_OVER, ID_DLG_NEWEL_LEFT_PUSHED,
                               ID_DLG_NEWEL_LEFT_NORMAL }, SpriteFileDialogNewElement, parent)
{
    SetShortcutKey(sgl::core::KeyboardEvent::KEY_LEFT);
}

void ButtonDialogArrowLeft::HandleMouseOver()
{
    sgl::sgui::AbstractButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-03.ogg");
}

void ButtonDialogArrowLeft::HandleButtonDown()
{
    sgl::sgui::AbstractButton::HandleButtonDown();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_click-03.ogg");
}

// ===== BUTTON RIGHT ARROW =====
ButtonDialogArrowRight::ButtonDialogArrowRight(sgl::sgui::Widget * parent)
    : sgl::sgui::ImageButton({ ID_DLG_NEWEL_RIGHT_NORMAL, ID_DLG_NEWEL_RIGHT_DISABLED,
                               ID_DLG_NEWEL_RIGHT_OVER, ID_DLG_NEWEL_RIGHT_PUSHED,
                               ID_DLG_NEWEL_RIGHT_NORMAL }, SpriteFileDialogNewElement, parent)
{
    SetShortcutKey(sgl::core::KeyboardEvent::KEY_RIGHT);
}

void ButtonDialogArrowRight::HandleMouseOver()
{
    sgl::sgui::AbstractButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-03.ogg");
}

void ButtonDialogArrowRight::HandleButtonDown()
{
    sgl::sgui::AbstractButton::HandleButtonDown();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_click-03.ogg");
}

} // namespace game
