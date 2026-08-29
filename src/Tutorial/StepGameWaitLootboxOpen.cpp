#include "Tutorial/StepGameWaitLootboxOpen.h"

#include "GameMap.h"

namespace game
{

StepGameWaitLootboxOpen::StepGameWaitLootboxOpen(const GameMap * gm, const GameObject * lootbox)
    : mGameMap(gm)
    , mLootbox(lootbox)
{
}

void StepGameWaitLootboxOpen::Update(float delta)
{
    TutorialStep::Update(delta);

    if(!mGameMap->HasObject(mLootbox))
        SetDone();
}

} // namespace game
