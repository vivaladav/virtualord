#include "GameObjects/ResearchCenter.h"

#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

ResearchCenter::ResearchCenter()
    : Structure(GameObject::TYPE_RESEARCH_CENTER, GameObject::CAT_GENERIC, 2, 2)
{
    SetVisibilityLevel(4);

    SetImage();
}

void ResearchCenter::UpdateGraphics()
{
    SetImage();

    SetDefaultColors();
}

void ResearchCenter::SetImage()
{
    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const unsigned int faction = GetFaction();
    const unsigned int sel = static_cast<unsigned int>(IsSelected());

    unsigned int texInd = ID_STRUCT_RESEARCH_CENTER;

    if(NO_FACTION == faction)
        texInd = ID_STRUCT_RESEARCH_CENTER + sel;
    else
        texInd = ID_STRUCT_RESEARCH_CENTER_F1 + (faction * NUM_RESEARCH_CENTER_SPRITES_PER_FAC) + sel;

    auto * tm = sgl::graphic::TextureManager::Instance();
    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileStructures, texInd);

    isoObj->SetTexture(tex);
}

} // namespace game
