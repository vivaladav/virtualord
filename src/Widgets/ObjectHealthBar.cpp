#include "ObjectHealthBar.h"

#include "GameData.h"

#include <sgl/graphic/Camera.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

ObjectHealthBar::ObjectHealthBar(unsigned int val)
{
    // ObjectHealthBar is part of the game scene
    SetCamera(sgl::graphic::Camera::GetDefaultCamera());

    SetValue(val);
}

void ObjectHealthBar::SetValue(unsigned int val)
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

void ObjectHealthBar::UpdateImage()
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();
    auto tex = tm->GetSprite(SpriteFileGameObjectsRelated, ID_GO_BAR_HEALTH_0 + mValue);
    SetTexture(tex);
}

} // namespace game
