#include "Tutorial/StepGameWaitEnemyKilled.h"

#include "GameMap.h"
#include "Widgets/Tutorial/PanelClickFilter.h"

namespace game
{

StepGameWaitEnemyKilled::StepGameWaitEnemyKilled(const GameObject * obj, const GameMap *gm)
    : mObj(obj)
    , mGameMap(gm)
    , mClickFilter(new PanelClickFilter)
{
    mClickFilter->SetEnabled(false);
}

StepGameWaitEnemyKilled::~StepGameWaitEnemyKilled()
{
    delete mClickFilter;
}

void StepGameWaitEnemyKilled::OnStart()
{
    mClickFilter->SetEnabled(true);
}

void StepGameWaitEnemyKilled::Update(float)
{
    if(!mGameMap->HasObject(mObj))
        SetDone();
}

} // namespace game
