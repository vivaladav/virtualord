#include "Tutorial/StepGameSetObjectAttackMode.h"

#include "GameObjects/GameObject.h"

namespace game
{

StepGameSetObjectAttackMode::StepGameSetObjectAttackMode(GameObject * obj, AttackMode mode)
    : mObj(obj)
    , mMode(mode)
{
}

void StepGameSetObjectAttackMode::OnStart()
{
    mObj->SetAttackMode(mMode);

    SetDone();
}

} // namespace game
