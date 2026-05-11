#include "Widgets/ButtonMainMenu.h"

#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>

namespace game
{

ButtonMainMenu::ButtonMainMenu(const char * text, sgl::sgui::Widget * parent)
    : GameButton(parent)
{
    using namespace sgl::graphic;

    SetData(SpriteFileMainMenuButtons,
            {
                IND_MM_BTN1_NORMAL,
                IND_MM_BTN1_DISABLED,
                IND_MM_BTN1_OVER,
                IND_MM_BTN1_PUSHED,
                IND_MM_BTN1_CHECKED,
            },
            {
                0xd6ebf5ff,
                0x395260ff,
                0x99e5ffff,
                0x8cd8f3ff,
                0xf0f099Ff
            });

    auto fm = FontManager::Instance();
    Font * font = fm->GetFont(WidgetsConstants::FontFileButton, 32, Font::NORMAL);

    SetLabelFont(font);

    SetLabel(text);
}

void ButtonMainMenu::HandleMouseOver()
{
    GameButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-01.ogg");
}

void ButtonMainMenu::HandleButtonDown()
{
    GameButton::HandleButtonDown();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_click-01.ogg");
}

} // namespace game
