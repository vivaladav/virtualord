#include "GameObjects/Hospital.h"

#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

Hospital::Hospital()
    : Structure(GameObject::TYPE_HOSPITAL, GameObject::CAT_GENERIC, 2, 2)
{
    SetVisibilityLevel(4);
}

void Hospital::UpdateGraphics()
{
    SetImage();

    SetDefaultColors();
}

void Hospital::SetImage()
{
    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const unsigned int faction = GetFaction();
    const unsigned int sel = static_cast<unsigned int>(IsSelected());

    unsigned int texInd = ID_STRUCT_HOSPITAL;

    if(NO_FACTION == faction)
        texInd = ID_STRUCT_HOSPITAL + sel;
    else
        texInd = ID_STRUCT_HOSPITAL_F1 + (faction * NUM_HOSPITAL_SPRITES_PER_FAC) + sel;

    auto * tm = sgl::graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);

    isoObj->SetTexture(tex);
}

} // namespace game
