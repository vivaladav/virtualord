#include "GameObjects/BlobsGenerator.h"

#include "GameConstants.h"
#include "GameMap.h"
#include "GameObjects/Blobs.h"

namespace game
{

BlobsGenerator::BlobsGenerator(GameMap * gm)
    : CollectableGenerator(gm, 5, 35)
{
}

void BlobsGenerator::OnGeneration()
{
    GameMap * gm = GetGameMap();

    const int r = GetRow();
    const int c = GetCol();

    GameObject * obj = gm->CreateObject(MapLayers::OBJECTS2, GameObject::TYPE_BLOBS,
                                        0, NO_FACTION, r, c, true);

    if(nullptr == obj)
        return ;

    // make the cell walkable for collection
    gm->SetCellWalkable(r, c, true);
}

} // namespace game
