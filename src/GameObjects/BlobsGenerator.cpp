#include "GameObjects/BlobsGenerator.h"

#include "GameObjects/ObjectData.h"

namespace game
{

BlobsGenerator::BlobsGenerator(const Game * g, GameMap * gm)
#ifdef DEV_MODE
    : CollectableGenerator(g, gm, 4, 12, ObjectData::TYPE_BLOBS)
#else
    : CollectableGenerator(g, gm, 5, 30, ObjectData::TYPE_BLOBS)
#endif
{
}

} // namespace game
