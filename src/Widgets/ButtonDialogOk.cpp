#include "Widgets/ButtonDialogOk.h"

#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>

namespace game
{

ButtonDialogOk::ButtonDialogOk(const char * text, sgl::sgui::Widget * parent)
        : GameButton(SpriteFileUIShared,
                     { ID_DLG_BTN_OK_NORMAL, ID_DLG_BTN_OK_DISABLED,
                      ID_DLG_BTN_OK_OVER, ID_DLG_BTN_OK_PUSHED,
                      ID_DLG_BTN_OK_PUSHED },
                     { WidgetsConstants::colorDialogButtonOkNormal,
                      WidgetsConstants::colorDialogButtonOkDisabled,
                      WidgetsConstants::colorDialogButtonOkOver,
                      WidgetsConstants::colorDialogButtonOkPushed,
                      WidgetsConstants::colorDialogButtonOkChecked }, parent)
{
    using namespace sgl;

    const int size = 22;

    auto fm = graphic::FontManager::Instance();

    auto fnt = fm->GetFont(WidgetsConstants::FontFileButton, size, graphic::Font::NORMAL);
    SetLabelFont(fnt);
    SetLabel(text);
}

void ButtonDialogOk::HandleMouseOver()
{
    sgl::sgui::AbstractButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-01.ogg");
}

void ButtonDialogOk::HandleButtonDown()
{
    sgl::sgui::AbstractButton::HandleButtonDown();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_click-01.ogg");
}

} // namespace game
