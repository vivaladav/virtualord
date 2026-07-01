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

        if(mGameMap->HasObject(obj))
            return ;

        ++it;
    }

    // no object found -> end
    SetDone();
}

} // namespace game
