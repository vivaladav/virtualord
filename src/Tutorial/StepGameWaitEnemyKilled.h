#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class GameMap;
class GameObject;
class PanelClickFilter;

class StepGameWaitEnemyKilled : public TutorialStep
{
public:
    StepGameWaitEnemyKilled(const GameObject * obj, const GameMap * gm);
    ~StepGameWaitEnemyKilled();

    void OnStart() override;

    void Update(float delta) override;

private:
    const GameObject * mObj = nullptr;
    const GameMap * mGameMap = nullptr;

    PanelClickFilter * mClickFilter = nullptr;
};

} // namespace game
