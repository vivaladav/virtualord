#include "GameObjects/Bunker.h"

#include "GameConstants.h"
#include "GameData.h"
#include "GameMap.h"
#include "IsoObject.h"
#include "GameObjects/ObjectData.h"
#include "GameObjectTools/Weapon.h"
#include "Screens/ScreenGame.h"

#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

Bunker::Bunker(const ObjectData & data, const ObjectInitData & initData)
    : Structure(data, initData)
{
    SetImage();
}

void Bunker::Update(float delta)
{
    GameObject::Update(delta);

    // do nothing if not linked
    if(!IsLinked())
        return ;

    mWeapon->Update(delta);

    if(mWeapon->IsReadyToShoot())
        PrepareShoot();
}

void Bunker::UpdateGraphics()
{
    SetImage();
}

void Bunker::SetImage()
{
    auto * tm = sgl::graphic::TextureManager::Instance();

    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const unsigned int faction = GetFaction();

    unsigned int texInd = ID_STRUCT_BUNKER;

    if(faction != NO_FACTION)
        texInd = ID_STRUCT_BUNKER_F1 + faction;

    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);
    isoObj->SetTexture(tex);
}

void Bunker::PrepareShoot()
{
    const GameObject * target = mWeapon->GetTarget();

    const float isoX = GetX();
    const float isoXC = GetX() + GetWidth() * 0.5f;
    const float isoY = GetY();
    const float isoTargetX = target->GetX();
    const float isoTargetY = target->GetY();
    const float x0 = isoTargetX < isoX ? isoXC - 20.f : isoXC + 20.f;
    const float y0 = isoTargetY < isoY ? isoY + 4 : isoY + 30;

    mWeapon->Shoot(x0, y0);
}

} // namespace game
