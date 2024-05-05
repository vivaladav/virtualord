#include "GameObjects/WallGate.h"

#include "GameConstants.h"
#include "GameData.h"
#include "GameMap.h"
#include "IsoObject.h"

#include <sgl/graphic/TextureManager.h>

namespace game
{

WallGate::WallGate(GameObjectVariantId orientation)
    : Structure(GameObject::TYPE_WALL_GATE, GameObject::CAT_GENERIC, 1, 1)
{
    mVariant = orientation;

    SetImage();
}

bool WallGate::Toggle()
{
    if(!IsLinked())
        return false;

    // toggle open value
    mOpen = !mOpen;

    // set cell walkable if open and not walkable otherwise
    const GameMapCell * cell = GetCell();
    GetGameMap()->SetCellWalkable(cell->row, cell->col, mOpen);

    ActionStepCompleted(TOGGLE_GATE);

    // update image
    SetImage();

    return true;
}

unsigned int WallGate::GetCostEnergy(unsigned int level)
{
    const unsigned int cost0 = 5;
    return (level + 1) * cost0;
}

unsigned int WallGate::GetCostMaterial(unsigned int level)
{
    const unsigned int cost0 = 10;
    return (level + 1) * cost0;
}

void WallGate::UpdateGraphics()
{
    SetImage();

    SetDefaultColors();
}

void WallGate::SetImage()
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
    const int ind0 = mVariant == HORIZ ? WALL_GATE_L1_F1_HORIZ_CLOSED : WALL_GATE_L1_F1_VERT_CLOSED;
    const int ind1 = ind0 + (NUM_SPRITES_PER_WALL_GATE_STATE * static_cast<int>(mOpen)) +
                     static_cast<int>(IsSelected());
    const int ind = ind1 + NUM_SPRITES_PER_WALL_GATE * faction;

    sgl::graphic::Texture * tex = tm->GetSprite(SpriteFileWalls, ind);
    isoObj->SetTexture(tex);
}

} // namespace game
