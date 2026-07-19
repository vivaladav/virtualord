#include "GameObjects/DefensiveTower.h"

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

const int maxAttVal = 11;
const int attRanges[maxAttVal] = { 0, 2, 3, 4, 5, 6, 7, 9, 10, 12, 14 };

DefensiveTower::DefensiveTower(const ObjectData & data, const ObjectInitData & initData)
    : Structure(data, initData)
{
    SetImage();
}

void DefensiveTower::Update(float delta)
{
    // do nothing if not linked
    if(!IsLinked())
        return ;

    mWeapon->Update(delta);

    if(mWeapon->IsReadyToShoot())
        PrepareShoot();
}

void DefensiveTower::UpdateGraphics()
{
    SetImage();
}

void DefensiveTower::SetImage()
{
    auto * tm = sgl::graphic::TextureManager::Instance();

    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const unsigned int faction = GetFaction();

    unsigned int texInd = ID_STRUCT_DTOWER_L1;

if(faction != NO_FACTION)
        texInd = ID_STRUCT_DTOWER_L1_F1 + faction;

    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);
    isoObj->SetTexture(tex);
}

void DefensiveTower::PrepareShoot()
{
    const IsoObject * isoObj = GetIsoObject();
    const IsoObject * isoTarget = mWeapon->GetTarget()->GetIsoObject();

    const float isoX = isoObj->GetX();
    const float isoXC = isoObj->GetX() + isoObj->GetWidth() * 0.5f;
    const float isoY = isoObj->GetY();
    const float isoTargetX = isoTarget->GetX();
    const float isoTargetY = isoTarget->GetY();
    const float x0 = isoTargetX < isoX ? isoXC - 20.f : isoXC + 20.f;
    const float y0 = isoTargetY < isoY ? isoY + 4 : isoY + 30;

    mWeapon->Shoot(x0, y0);
}

} // namespace game
