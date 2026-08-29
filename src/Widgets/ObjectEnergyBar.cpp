#include "ObjectEnergyBar.h"

#include "GameData.h"

#include <sgl/graphic/Camera.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

ObjectEnergyBar::ObjectEnergyBar(unsigned int val)
{
    // ObjectEnergyBar is part of the game scene
    SetCamera(sgl::graphic::Camera::GetDefaultCamera());

    SetValue(val);
}

void ObjectEnergyBar::SetValue(unsigned int val)
{
    // clamp new value
    if(val > MAX_VAL)
        val = MAX_VAL;

    // do nothing if setting same value
    if(mValue == val)
        return;

    mValue = val;

    UpdateImage();
}

void ObjectEnergyBar::UpdateImage()
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();
    auto tex = tm->GetSprite(SpriteFileGameObjectsRelated, ID_GO_BAR_ENERGY_0 + mValue);
    SetTexture(tex);
}

} // namespace game
