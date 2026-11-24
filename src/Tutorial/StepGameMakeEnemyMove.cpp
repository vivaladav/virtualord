#include "Tutorial/StepGameMakeEnemyMove.h"

#include "Player.h"
#include "AI/PlayerAI.h"
#include "GameObjects/Unit.h"
#include "GameObjectTools/Weapon.h"

namespace game
{

StepGameMakeEnemyMove::StepGameMakeEnemyMove(Unit * unit, const Cell2D & target, Player * playerAI)
    : mUnit(unit)
    , mTarget(target)
    , mPlayerAI(playerAI)
{
}

void StepGameMakeEnemyMove::OnStart()
{
    TutorialStep::OnStart();

    auto ai = mPlayerAI->GetAI();

    // CREATE AI ACTION
    auto action = new ActionAI;
    action->type = AIA_UNIT_PATROL;
    action->ObjSrc = mUnit;
    action->cellDst = mTarget;
    action->priority = PlayerAI::MAX_PRIORITY;

    // push action to the queue
    ai->RequestNewAction(action);

    SetDone();
}

} // namespace game
