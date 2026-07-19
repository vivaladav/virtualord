#include "GameObjects/RadarStation.h"

#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

RadarStation::RadarStation(const ObjectData & data, const ObjectInitData & initData)
    : Structure(data, initData)
{
    SetCanBeConquered(true);

    SetImage();
}

void RadarStation::UpdateGraphics()
{
    SetImage();
}

void RadarStation::SetImage()
{
    auto * tm = sgl::graphic::TextureManager::Instance();

    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const PlayerFaction faction = GetFaction();

    unsigned int texInd = ID_STRUCT_RADAR;

    if(NO_FACTION == faction)
        texInd = ID_STRUCT_RADAR;
    else
        texInd = ID_STRUCT_RADAR_F1 + faction;

    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);
    isoObj->SetTexture(tex);
}

} // namespace game

