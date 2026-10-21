#pragma once

#include "Tutorial/TutorialStep.h"

#include "Cell2D.h"

namespace game
{

class GameObject;
class Player;
class Unit;

class StepGameMakeEnemyMove : public TutorialStep
{
public:
    StepGameMakeEnemyMove(Unit * unit, const Cell2D & target, Player * playerAI);

    void OnStart() override;

private:
    Unit * mUnit = nullptr;
    const Cell2D mTarget;
    Player * mPlayerAI = nullptr;
};

} // namespace game
