#include "GameObjects/CollectableGenerator.h"

#include "GameMap.h"

#include <sgl/utilities/UniformDistribution.h>

namespace game
{

CollectableGenerator::CollectableGenerator(GameMap * gm, int turnsMin, int turnsMax)
    : mGameMap(gm)
{
    ResetCounter(turnsMin, turnsMax);
}

CollectableGenerator::~CollectableGenerator()
{
}

void CollectableGenerator::OnNewTurn()
{
    // generate only if cell is empty
    if(mGameMap->HasObject(mRow, mCol))
        return ;

    --mCounterRegen;

    // nothing to do until counter is 0
    if(mCounterRegen > 0)
        return ;

    // reset counter
    mCounterRegen = mRegenTurns;

    // generate
    OnGeneration();
}

void CollectableGenerator::ResetCounter(int min, int max)
{
    // randomize generation time between min and max
    sgl::utilities::UniformDistribution ran(min, max);

    mRegenTurns = ran.GetNextValue();
    mCounterRegen = mRegenTurns;
}

} // namespace game
