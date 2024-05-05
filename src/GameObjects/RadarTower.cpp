#include "GameObjects/RadarTower.h"

#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

RadarTower::RadarTower()
    : Structure(TYPE_RADAR_TOWER, CAT_GENERIC, 1, 1)
{
    SetCanBeConquered(true);

    SetImage();

    SetVisibilityLevel(1);
}

void RadarTower::UpdateGraphics()
{
    SetImage();

    SetDefaultColors();
}

void RadarTower::OnLinkedChanged()
{
    Structure::OnLinkedChanged();

    const int maxVis = 9;
    SetVisibilityLevel(maxVis * static_cast<int>(IsLinked()));
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
    const unsigned int sel = static_cast<unsigned int>(IsSelected());

    unsigned int texInd = ID_STRUCT_RADAR_TOWER;

    if(NO_FACTION == faction)
        texInd = ID_STRUCT_RADAR_TOWER + sel;
    else
        texInd = ID_STRUCT_RADAR_TOWER_F1 + (faction * NUM_RADAR_TOWER_SPRITES_PER_FAC) + sel;

    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);
    isoObj->SetTexture(tex);
}

} // namespace game

