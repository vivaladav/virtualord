#include "BlinkingIconEnergy.h"

#include "GameData.h"

#include <sgl/graphic/Camera.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

const float TIME_BLINK_ON = 0.5f;
const float TIME_BLINK_OFF = 0.4f;

BlinkingIconEnergy::BlinkingIconEnergy()
    : mTimerBlink(TIME_BLINK_ON)
{
    using namespace sgl;

    // BlinkingIconEnergy is part of the game scene
    SetCamera(graphic::Camera::GetDefaultCamera());

    // ICON
    const unsigned int colorIcon = 0xe4cb81ff;

    auto tm = graphic::TextureManager::Instance();
    auto tex = tm->GetSprite(SpriteFileParticles, ID_PART_ICON_ENERGY);
    SetTexture(tex);
    SetColor(colorIcon);
}

void BlinkingIconEnergy::OnUpdate(float delta)
{
    mTimerBlink -= delta;

    if(mTimerBlink > 0.f)
        return ;

    SetVisible(!IsVisible());

    mTimerBlink = IsVisible() ? TIME_BLINK_ON : TIME_BLINK_OFF;
}

} // namespace game
