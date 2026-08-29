#include "GameObjects/SpawningTower.h"

#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"
#include "ObjectData.h"

#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

SpawningTower::SpawningTower(const ObjectData & data, const ObjectInitData & initData)
    : Structure(data, initData)
{
    SetImage();
}

float SpawningTower::GetTimeSpawnMiniUnit() const
{
    const float maxTime = 2.f;
    return GetTime(maxTime, GetAttribute(OBJ_ATT_SPAWNING));
}

void SpawningTower::UpdateGraphics()
{
    SetImage();
}

void SpawningTower::SetImage()
{
    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const unsigned int faction = GetFaction();

    unsigned int texInd = ID_STRUCT_SPAWN_TOWER;

    if(faction != NO_FACTION)
        texInd = ID_STRUCT_SPAWN_TOWER_F1 + faction;

    auto * tm = sgl::graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);

    isoObj->SetTexture(tex);
}

} // namespace game
