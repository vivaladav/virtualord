#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class GameObject;
class Player;
class Unit;

class StepGameMakeEnemyAttack : public TutorialStep
{
public:
    StepGameMakeEnemyAttack(Unit * attacker, GameObject * target, Player * attackPlayer);

    void OnStart() override;

private:
    Unit * mAttacker = nullptr;
    GameObject * mTarget = nullptr;
    Player * mAttackPlayer = nullptr;
};

} // namespace game
