#include "GameObjects/BlobsGenerator.h"

#include "GameConstants.h"
#include "GameMap.h"
#include "GameObjects/Blobs.h"

namespace game
{

BlobsGenerator::BlobsGenerator(const Game * g, GameMap * gm)
#ifdef DEV_MODE
    : CollectableGenerator(g, gm, 4, 12, ObjectData::TYPE_BLOBS)
#else
    : CollectableGenerator(g, gm, 5, 30)
#endif
{
}

void BlobsGenerator::OnGeneration()
{
    GameMap * gm = GetGameMap();

    const int r = GetRow();
    const int c = GetCol();

    GameObject * obj = gm->CreateObject(MapLayers::REGULAR_OBJECTS, ObjectData::TYPE_BLOBS,
                                        0, NO_FACTION, r, c, true);

    if(nullptr == obj)
        return ;

    // make the cell walkable for collection
    gm->SetCellWalkable(r, c, true);
}

} // namespace game
