#include "GameObjects/DiamondsGenerator.h"

#include "GameConstants.h"
#include "GameMap.h"
#include "GameObjects/Diamonds.h"

namespace game
{

DiamondsGenerator::DiamondsGenerator(GameMap * gm)
    : CollectableGenerator(gm, 4, 30)
{
}

void DiamondsGenerator::OnGeneration()
{
    GameMap * gm = GetGameMap();

    const int r = GetRow();
    const int c = GetCol();

    GameObject * obj = gm->CreateObject(MapLayers::OBJECTS2, GameObject::TYPE_DIAMONDS,
                                        0, NO_FACTION, r, c, true);

    if(nullptr == obj)
        return ;

    // make the cell walkable for collection
    gm->SetCellWalkable(r, c, true);
}

} // namespace game
