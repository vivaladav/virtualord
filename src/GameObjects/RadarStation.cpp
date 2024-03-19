#include "GameObjects/RadarStation.h"

#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

RadarStation::RadarStation()
    : Structure(TYPE_RADAR_STATION, CAT_GENERIC, 2, 2)
{
    SetCanBeConquered(true);

    SetImage();

    SetVisibilityLevel(1);
}

void RadarStation::UpdateGraphics()
{
    SetImage();

    SetDefaultColors();
}

void RadarStation::OnLinkedChanged()
{
    Structure::OnLinkedChanged();

    const bool linked = IsLinked();

    // update object visibility level
    const int maxVis = 15;
    SetVisibilityLevel(maxVis * static_cast<int>(linked));
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
    const unsigned int sel = static_cast<unsigned int>(IsSelected());

    unsigned int texInd = ID_STRUCT_RADAR;

    if(NO_FACTION == faction)
        texInd = ID_STRUCT_RADAR + sel;
    else
        texInd = ID_STRUCT_RADAR_F1 + (faction * NUM_RADAR_SPRITES_PER_FAC) + sel;

    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);
    isoObj->SetTexture(tex);
}

} // namespace game

