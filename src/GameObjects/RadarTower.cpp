#include "GameObjects/RadarTower.h"

#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

RadarTower::RadarTower(const ObjectData & data, const ObjectInitData & initData)
    : Structure(data, initData)
{
    SetCanBeConquered(true);

    SetImage();
}

void RadarTower::UpdateGraphics()
{
    SetImage();
}

void RadarTower::SetImage()
{
    auto * tm = sgl::graphic::TextureManager::Instance();

    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const PlayerFaction faction = GetFaction();

    unsigned int texInd = ID_STRUCT_RADAR_TOWER;

    if(NO_FACTION == faction)
        texInd = ID_STRUCT_RADAR_TOWER;
    else
        texInd = ID_STRUCT_RADAR_TOWER_F1 + faction;

    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);
    isoObj->SetTexture(tex);
}

} // namespace game

