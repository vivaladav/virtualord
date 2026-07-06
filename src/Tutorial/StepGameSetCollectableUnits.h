#pragma once

#include "Cell2D.h"
#include "GameObjects/GameObjectTypes.h"
#include "Tutorial/TutorialStep.h"

namespace game
{

class GameMap;
class GameObject;

class StepGameSetCollectableUnits : public TutorialStep
{
public:
    StepGameSetCollectableUnits(const Cell2D & tl, const Cell2D & br, const GameMap * gm,
                               GameObjectTypeId type, int min, int max);

    void OnStart() override;

private:
    Cell2D mTL;
    Cell2D mBR;

    const GameMap * mGameMap = nullptr;

    GameObjectTypeId mGenType;
    int mMin = 0;
    int mMax = 0;
};

} // namespace game
