#include "GameObjects/Wall.h"

#include "GameConstants.h"
#include "GameData.h"
#include "IsoObject.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

Wall::Wall(const ObjectData & data, const ObjectInitData & initData, GameObjectVariantId part)
    : Structure(data, initData)
{
    mVariant = part;

    SetImage();
}

unsigned int Wall::GetCostEnergy(unsigned int level)
{
    const unsigned int cost0 = 10;
    return (level + 1) * cost0;
}

unsigned int Wall::GetCostMaterial(unsigned int level)
{
    const unsigned int cost0 = 20;
    return (level + 1) * cost0;
}

void Wall::UpdateGraphics()
{
    SetImage();
}

void Wall::SetImage()
{
    auto * tm = sgl::graphic::TextureManager::Instance();

    IsoObject * isoObj = GetIsoObject();

    if(IsVisible())
        isoObj->SetColor(COLOR_VIS);
    else
        isoObj->SetColor(COLOR_FOW);

    const PlayerFaction faction = GetFaction();

    // avoid to set an image when there's no owner set
    if(NO_FACTION == faction)
        return ;

    // set texture
    const int ind = SpriteWallsId::WALL_L1_F1_HORIZ + mVariant + (faction * NUM_WALL_PARTS);

    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileWalls, ind);
    isoObj->SetTexture(tex);
}

} // namespace game
