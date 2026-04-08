#include "ButtonPlanetMap.h"

#include "Widgets/GameSimpleTooltip.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

// -- PRIMARY BUTTON --
ButtonPlanetMap::ButtonPlanetMap(sgl::sgui::Widget * parent) :
    GameButton(SpriteFilePlanetMap,
        { IND_PM_BTN_NORMAL, IND_PM_BTN_DISABLED, IND_PM_BTN_OVER,
          IND_PM_BTN_PRESSED, IND_PM_BTN_NORMAL },
        { 0xd7eaf4ff, 0x335f73b2, 0xebf4f9ff, 0xc3dfeeff, 0xd7eaf4ff },
        parent)
{
    using namespace sgl;

    const int size = 20;

    auto fm = graphic::FontManager::Instance();
    graphic::Font * fnt = fm->GetFont(WidgetsConstants::FontFileButton, size, graphic::Font::NORMAL);
    SetLabelFont(fnt);
}

void ButtonPlanetMap::SetTooltipText(const char * text)
{
    if(mTooltip == nullptr)
        CreateTooltip(text);
    else
        mTooltip->SetText(text);
}

void ButtonPlanetMap::HandleMouseOver()
{
    sgl::sgui::AbstractButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-01.ogg");
}

void ButtonPlanetMap::HandleButtonDown()
{
    sgl::sgui::AbstractButton::HandleButtonDown();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_click-01.ogg");
}

void ButtonPlanetMap::CreateTooltip(const char * text)
{
    const int timeShowing = 2500;

    mTooltip = new GameSimpleTooltip(text);
    SetTooltip(mTooltip);
    SetTooltipDelay(WidgetsConstants::timeTooltipButtonDelay);
    SetTooltipShowingTime(timeShowing);
}

// -- SECONDARY BUTTON --
SecondaryButtonPlanetMap::SecondaryButtonPlanetMap(sgl::sgui::Widget * parent) :
    GameButton(SpriteFilePlanetMap,
        { IND_PM_BTN2_NORMAL, IND_PM_BTN2_DISABLED, IND_PM_BTN2_OVER,
          IND_PM_BTN2_PRESSED, IND_PM_BTN2_NORMAL },
        { 0xf4e5d7ff, 0x735333b2, 0xf9f2ebff, 0xeed9c3ff, 0xf4e5d7ff },
        parent)
{
    using namespace sgl;

    const int size = 20;

    auto fm = graphic::FontManager::Instance();
    graphic::Font * fnt = fm->GetFont(WidgetsConstants::FontFileButton, size, graphic::Font::NORMAL);
    SetLabelFont(fnt);
}

void SecondaryButtonPlanetMap::HandleMouseOver()
{
    sgl::sgui::AbstractButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-01.ogg");
}

void SecondaryButtonPlanetMap::HandleButtonDown()
{
    sgl::sgui::AbstractButton::HandleButtonDown();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_click_cancel-01.ogg");
}

// -- BUTTON LEAVE PLANET --
ButtonLeavePlanet::ButtonLeavePlanet(sgl::sgui::Widget * parent)
    : GameButton(SpriteFilePlanetMap,
                 { IND_PM_BTN3_NORMAL, IND_PM_BTN3_DISABLED, IND_PM_BTN3_OVER,
                   IND_PM_BTN3_PRESSED, IND_PM_BTN3_NORMAL },
                 { 0xf2ddd9ff, 0x633d36ff, 0xf9eeecff, 0xecccc6ff, 0xf2ddd9ff },
                 parent)
{
    using namespace sgl;

    auto sm = utilities::StringManager::Instance();

    const int size = 20;

    auto fm = graphic::FontManager::Instance();
    graphic::Font * fnt = fm->GetFont(WidgetsConstants::FontFileButton, size, sgl::graphic::Font::NORMAL);
    SetLabelFont(fnt);

    SetLabel(sm->GetCString("LEAVE_PLANET"));
}

void ButtonLeavePlanet::HandleMouseOver()
{
    sgl::sgui::AbstractButton::HandleMouseOver();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_over-01.ogg");
}

void ButtonLeavePlanet::HandleButtonDown()
{
    sgl::sgui::AbstractButton::HandleButtonDown();

    auto player = sgl::media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/button_click-01.ogg");
}

void ButtonLeavePlanet::OnStringsChanged()
{
    auto sm = sgl::utilities::StringManager::Instance();

    SetLabel(sm->GetCString("LEAVE_PLANET"));
}

} // namespace game
