#include "GameObjects/DiamondsGenerator.h"

#include "GameConstants.h"
#include "GameMap.h"
#include "GameObjects/Diamonds.h"

namespace game
{

DiamondsGenerator::DiamondsGenerator(const Game * g, GameMap * gm)
#ifdef DEV_MODE
    : CollectableGenerator(g, gm, 4, 12, ObjectData::TYPE_DIAMONDS)
#else
    : CollectableGenerator(g, gm, 5, 30, ObjectData::TYPE_DIAMONDS)
#endif
{
}

void DiamondsGenerator::OnGeneration()
{
    GameMap * gm = GetGameMap();

    const int r = GetRow();
    const int c = GetCol();

    GameObject * obj = gm->CreateObject(MapLayers::REGULAR_OBJECTS, ObjectData::TYPE_DIAMONDS,
                                        0, NO_FACTION, r, c, true);

    if(nullptr == obj)
        return ;

    // make the cell walkable for collection
    gm->SetCellWalkable(r, c, true);
}

} // namespace game
