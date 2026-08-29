#include "Tutorial/StepGameWaitEnemiesKilled.h"

#include "GameMap.h"
#include "Widgets/Tutorial/PanelClickFilter.h"

namespace game
{

StepGameWaitEnemiesKilled::StepGameWaitEnemiesKilled(const std::vector<const GameObject *> && objs,
                                                     const GameMap * gm)
    : mObjs(objs)
    , mGameMap(gm)
{
}

void StepGameWaitEnemiesKilled::OnStart()
{
    TutorialStep::OnStart();

    // disable click filter as interaction is needed while updating this Step
    GetClickFilter()->SetEnabled(false);
}

void StepGameWaitEnemiesKilled::Update(float)
{
    auto it = mObjs.begin();

    while(it != mObjs.end())
    {
        const GameObject * obj = *it;

        // found one -> not done yet
        if(mGameMap->HasObject(obj))
            ++it;
        // if not found remove it from the list
        else
            it = mObjs.erase(it);
    }

    // no object found -> end
    if(mObjs.empty())
        SetDone();
}

} // namespace game
