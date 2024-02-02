#include "Widgets/ButtonMinimap.h"

#include "Widgets/GameUIData.h"

#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>

namespace game

{

const std::array<unsigned int, sgl::sgui::PushButton::NUM_VISUAL_STATES> texIds =
{
    IND_MINIMAP_BUTTON_NORMAL,
    IND_MINIMAP_BUTTON_NORMAL,
    IND_MINIMAP_BUTTON_OVER,
    IND_MINIMAP_BUTTON_PUSHED,
    IND_MINIMAP_BUTTON_NORMAL
};

ButtonMinimap::ButtonMinimap(sgl::sgui::Widget * parent)
    : sgl::sgui::ImageButton(texIds, SpriteFilePanelMinimap, parent)
{
}

void ButtonMinimap::HandleMouseOver()
{
    sgl::sgui::AbstractButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-03.ogg");
}

void ButtonMinimap::HandleButtonDown()
{
    sgl::sgui::AbstractButton::HandleButtonDown();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/dialog_open-02.ogg");
}

} // namespace game
