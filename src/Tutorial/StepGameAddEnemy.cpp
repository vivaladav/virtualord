#include "Tutorial/StepGameAddEnemy.h"

#include "GameMap.h"
#include "Player.h"

namespace game
{

StepGameAddEnemy::StepGameAddEnemy(GameMap * gm, Player * owner, GameObjectTypeId type,
                                   const Cell2D & target, bool dummy)
    : mGameMap(gm)
    , mOwner(owner)
    , mTarget(target)
    , mType(type)
    , mDummy(dummy)
{
}

void StepGameAddEnemy::OnStart()
{
    const GameMapCell & gmCell = mGameMap->GetCell(mTarget.row, mTarget.col);

    if(gmCell.objTop == nullptr && gmCell.objBottom == nullptr)
    {
        auto unit = mGameMap->CreateUnit(mType, mTarget, mOwner);

        if(mDummy)
            mOwner->RemoveUnit(unit);
    }

    SetDone();
}

} // namespace game
