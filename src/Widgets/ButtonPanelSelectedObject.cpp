#include "Widgets/ButtonPanelSelectedObject.h"

#include "Widgets/GameUIData.h"

#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>

namespace game

{

const std::array<unsigned int, sgl::sgui::PushButton::NUM_VISUAL_STATES> texIds =
{
    ID_PAN_SELOBJ_BTN_NORMAL,
    ID_PAN_SELOBJ_BTN_NORMAL,
    ID_PAN_SELOBJ_BTN_OVER,
    ID_PAN_SELOBJ_BTN_PUSHED,
    ID_PAN_SELOBJ_BTN_NORMAL
};

ButtonPanelSelectedObject::ButtonPanelSelectedObject(sgl::sgui::Widget * parent)
    : sgl::sgui::ImageButton(texIds, SpriteFilePanelSelectedObject, parent)
{
}

void ButtonPanelSelectedObject::HandleMouseOver()
{
    sgl::sgui::AbstractButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-03.ogg");
}

void ButtonPanelSelectedObject::HandleButtonDown()
{
    sgl::sgui::AbstractButton::HandleButtonDown();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/dialog_open-02.ogg");
}

} // namespace game
