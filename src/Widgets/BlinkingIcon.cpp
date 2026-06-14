#include "BlinkingIcon.h"

#include "Widgets/GameUIData.h"

#include <sgl/graphic/Camera.h>
#include <sgl/graphic/TextureManager.h>

namespace
{
constexpr float TIME_BLINK_ON = 0.5f;
constexpr float TIME_BLINK_OFF = 0.4f;

}

namespace game
{

// ===== BASE BLINKING ICON =====
BlinkingIcon::BlinkingIcon(const char * file, unsigned int spriteId)
    : mTimerBlink(TIME_BLINK_ON)
{
    // BlinkingIcon is part of the game scene
    SetCamera(sgl::graphic::Camera::GetDefaultCamera());

    // ICON
    auto tm = sgl::graphic::TextureManager::Instance();
    auto tex = tm->GetSprite(file, spriteId);
    SetTexture(tex);
}

void BlinkingIcon::OnUpdate(float delta)
{
    mTimerBlink -= delta;

    if(mTimerBlink > 0.f)
        return ;

    SetVisible(!IsVisible());

    mTimerBlink = IsVisible() ? TIME_BLINK_ON : TIME_BLINK_OFF;
}

// ===== ENERGY =====
BlinkingIconEnergy::BlinkingIconEnergy()
    : BlinkingIcon(SpriteFileUIShared, ID_UIS_ICON_C_RES_ENERGY_24)
{
}

// ===== RESEARCH =====
BlinkingIconResearch::BlinkingIconResearch()
    : BlinkingIcon(SpriteFileUIShared, ID_UIS_ICON_C_RES_RESEARCH_24)
{
}

} // namespace game
