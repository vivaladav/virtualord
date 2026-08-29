#include "Tutorial/StepGameWaitEnemyKilled.h"

#include "GameMap.h"

namespace game
{

StepGameWaitEnemyKilled::StepGameWaitEnemyKilled(const GameObject * obj, const GameMap *gm)
    : mObj(obj)
    , mGameMap(gm)
{
}

void StepGameWaitEnemyKilled::Update(float)
{
    if(!mGameMap->HasObject(mObj))
        SetDone();
}

} // namespace game
