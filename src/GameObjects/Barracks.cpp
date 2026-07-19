#include "GameObjects/Barracks.h"

#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

Barracks::Barracks(const ObjectData & data, const ObjectInitData & initData)
    : Structure(data, initData)
{
    SetImage();
}

void Barracks::UpdateGraphics()
{
    SetImage();
}

void Barracks::SetImage()
{
    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const unsigned int faction = GetFaction();

    unsigned int texInd = ID_STRUCT_BARRACKS;

    if(NO_FACTION == faction)
        texInd = ID_STRUCT_BARRACKS;
    else
        texInd = ID_STRUCT_BARRACKS_F1 + (faction * NUM_BARRACKS_SPRITES_PER_FAC);

    auto * tm = sgl::graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);

    isoObj->SetTexture(tex);
}

} // namespace game
