#include "GameObjects/CollectableGenerator.h"

#include "Game.h"
#include "GameConstants.h"
#include "GameMap.h"

#include <sgl/utilities/BinaryFile.h>
#include <sgl/utilities/UniformDistribution.h>

namespace game
{

CollectableGenerator::CollectableGenerator(const Game * g, GameMap * gm,
                                           GameObjectTypeId productType)
    : mGame(g)
    , mGameMap(gm)
    , mProductType(productType)
{
}

CollectableGenerator::CollectableGenerator(const Game * g, GameMap * gm, int turnsMin, int turnsMax,
                                           GameObjectTypeId productType)
    : mGame(g)
    , mGameMap(gm)
    , mProductType(productType)
{
    ResetCounter(turnsMin, turnsMax);
}

CollectableGenerator::~CollectableGenerator()
{
}

bool CollectableGenerator::Load(sgl::utilities::BinaryFile & bf)
{
    mRegenTurns = bf.ReadInt();
    mCounterRegen = bf.ReadInt();

    return true;
}

bool CollectableGenerator::Save(sgl::utilities::BinaryFile & bf) const
{
    bf.WriteInt(mRegenTurns);
    bf.WriteInt(mCounterRegen);

    return true;
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
    sgl::utilities::UniformDistribution ran(min, max, mGame->GetRandSeed());

    mRegenTurns = ran.GetNextValue();
    mCounterRegen = mRegenTurns;
}

void CollectableGenerator::OnGeneration()
{
    GameMap * gm = GetGameMap();

    const int r = GetRow();
    const int c = GetCol();

    GameObject * obj = gm->CreateObject(GetProductType(), 0, NO_FACTION, r, c, true);

    if(nullptr == obj)
        return ;

    // make the cell walkable for collection
    gm->SetCellWalkable(r, c, true);
}

} // namespace game
