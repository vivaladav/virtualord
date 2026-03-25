#include "Widgets/ButtonPanelTab.h"

#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>


namespace game
{

ButtonPanelTab::ButtonPanelTab(const char * text, sgl::sgui::Widget * parent)
    : GameButton(SpriteFileUIShared,
                 { ID_BTN_TAB_NORMAL, ID_BTN_TAB_DISABLED,
                   ID_BTN_TAB_OVER, ID_BTN_TAB_PUSHED,
                   ID_BTN_TAB_CHECKED },
                 { WidgetsConstants::colorDialogButtonNormal,
                  WidgetsConstants::colorDialogButtonDisabled,
                  WidgetsConstants::colorDialogButtonOver,
                  WidgetsConstants::colorDialogButtonPushed,
                  WidgetsConstants::colorDialogButtonChecked }, parent)
{
    using namespace sgl;

    const int size = 20;

    auto fm = graphic::FontManager::Instance();

    auto fnt = fm->GetFont(WidgetsConstants::FontFileButton, size, graphic::Font::NORMAL);
    SetLabelFont(fnt);
    SetLabel(text);
}


void ButtonPanelTab::HandleMouseOver()
{
    sgl::sgui::AbstractButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-03.ogg");
}

void ButtonPanelTab::HandleButtonDown()
{
    sgl::sgui::AbstractButton::HandleButtonDown();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_click-03.ogg");
}

} // namespace game
