#include "Tutorial/StepGameWaitEnemiesKilled.h"

#include "GameMap.h"

namespace game
{

StepGameWaitEnemiesKilled::StepGameWaitEnemiesKilled(const std::vector<const GameObject *> & objs,
                                                     const GameMap * gm)
    : mObjs(objs)
    , mGameMap(gm)
{
}

void StepGameWaitEnemiesKilled::Update(float)
{
    auto it = mObjs.begin();

    while(it != mObjs.end())
    {
        const GameObject * obj = *it;

        // found one -> not done yet
        if(mGameMap->HasObject(obj))
            return ;
        // if not found remove it from the list
        else
            it = mObjs.erase(it);
    }

    // no object found -> end
    SetDone();
}

} // namespace game
