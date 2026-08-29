#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class GameMap;
class GameObject;

class StepGameWaitLootboxOpen : public TutorialStep
{
public:
    StepGameWaitLootboxOpen(const GameMap * gm, const GameObject * lootbox);

    void Update(float delta) override;

private:
    const GameMap * mGameMap = nullptr;
    const GameObject * mLootbox = nullptr;
};

} // namespace game
