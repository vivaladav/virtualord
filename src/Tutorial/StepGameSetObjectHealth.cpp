#include "Tutorial/StepGameSetObjectHealth.h"

#include "GameObjects/GameObject.h"

namespace game
{

StepGameSetObjectHealth::StepGameSetObjectHealth(GameObject * obj, float val)
    : mObj(obj)
    , mVal(val)
{
}

void StepGameSetObjectHealth::OnStart()
{
    const float health0 = mObj->GetHealth();
    const float delta = mVal - health0;

    mObj->SumHealth(delta);

    SetDone();
}

} // namespace game
