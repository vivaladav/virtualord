#pragma once

#include "Tutorial/TutorialStep.h"

#include "GameObjects/GameObjectTypes.h"
#include "Cell2D.h"

namespace game
{

class GameMap;
class Player;

class StepGameAddEnemy : public TutorialStep
{
public:
    StepGameAddEnemy(GameMap * gm, Player * owner, GameObjectTypeId type,
                     const Cell2D & target, bool dummy);

    void OnStart() override;

private:
    GameMap * mGameMap = nullptr;
    Player * mOwner = nullptr;
    const Cell2D mTarget;
    GameObjectTypeId mType;
    bool mDummy;
};

} // namespace game
