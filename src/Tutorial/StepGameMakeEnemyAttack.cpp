#include "Tutorial/StepGameMakeEnemyAttack.h"

#include "Player.h"
#include "AI/PlayerAI.h"
#include "GameObjects/Unit.h"
#include "GameObjectTools/Weapon.h"

namespace game
{

StepGameMakeEnemyAttack::StepGameMakeEnemyAttack(Unit * attacker, GameObject * target,
                                                 Player * attackPlayer)
    : mAttacker(attacker)
    , mTarget(target)
    , mAttackPlayer(attackPlayer)
{
}

void StepGameMakeEnemyAttack::OnStart()
{
    TutorialStep::OnStart();

    auto ai = mAttackPlayer->GetAI();

    // CREATE AI ACTION
    auto action = new ActionAI;
    action->type = AIA_UNIT_ATTACK_ENEMY_UNIT;
    action->ObjSrc = mAttacker;
    action->ObjDst = mTarget;
    action->priority = PlayerAI::MAX_PRIORITY;

    // push action to the queue
    ai->RequestNewAction(action);

    SetDone();
}

} // namespace game
