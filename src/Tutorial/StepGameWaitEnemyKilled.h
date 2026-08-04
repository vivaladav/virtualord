#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class GameMap;
class GameObject;

class StepGameWaitEnemyKilled : public TutorialStep
{
public:
    StepGameWaitEnemyKilled(const GameObject * obj, const GameMap * gm);

    void Update(float delta) override;

private:
    const GameObject * mObj = nullptr;
    const GameMap * mGameMap = nullptr;
};

} // namespace game
