#pragma once

#include "Tutorial/TutorialStep.h"

#include <vector>

namespace game
{

class GameMap;
class GameObject;

class StepGameWaitEnemiesKilled : public TutorialStep
{
public:
    StepGameWaitEnemiesKilled(const std::vector<const GameObject *> && objs, const GameMap * gm);

    void OnStart() override;

    void Update(float delta) override;

private:
    std::vector<const GameObject *> mObjs;

    const GameMap * mGameMap = nullptr;
};

} // namespace game
