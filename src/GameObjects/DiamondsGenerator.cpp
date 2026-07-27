#include "GameObjects/DiamondsGenerator.h"

#include "GameObjects/ObjectData.h"

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

} // namespace game
