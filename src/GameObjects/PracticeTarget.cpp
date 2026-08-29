#include "GameObjects/PracticeTarget.h"

#include "GameData.h"
#include "IsoObject.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

PracticeTarget::PracticeTarget(const ObjectData & data, const ObjectInitData & initData)
    : Structure(data, initData)
{
    // health
    const float maxHealthValue = 2500.f;
    SetMaxHealth(maxHealthValue);

    SetImage();

    SetObjColors();
}

void PracticeTarget::UpdateGraphics()
{
    SetImage();
}

void PracticeTarget::SetImage()
{
    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    // set texture
    const unsigned int spriteId = ID_PRACTICE_TARGET;
    auto tm = sgl::graphic::TextureManager::Instance();

    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, spriteId);
    isoObj->SetTexture(tex);
}

void PracticeTarget::SetObjColors()
{
    mObjColors.clear();

    mObjColors.push_back(0xb2b2b2ff);
    mObjColors.push_back(0xc06158ff);
    mObjColors.push_back(0xa7483fff);
    mObjColors.push_back(0x74423fff);
    mObjColors.push_back(0x603c39ff);
    mObjColors.push_back(0x503230ff);
}

} // namespace game
