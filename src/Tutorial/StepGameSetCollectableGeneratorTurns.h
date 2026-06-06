#pragma once

#include "GameObjects/GameObjectTypes.h"
#include "Tutorial/TutorialStep.h"

namespace game
{

class GameMap;
class GameObject;

class StepGameSetCollectableGeneratorTurns : public TutorialStep
{
public:
    StepGameSetCollectableGeneratorTurns(GameMap * gm, GameObjectTypeId type, int min, int max);

    void OnStart() override;

private:
    GameMap * mGameMap = nullptr;

    GameObjectTypeId mGenType;
    int mMin = 0;
    int mMax = 0;
};

} // namespace game
