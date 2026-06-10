#include "Tutorial/StepGameSetObjectEnergy.h"

#include "GameObjects/GameObject.h"

namespace game
{

StepGameSetObjectEnergy::StepGameSetObjectEnergy(GameObject * obj, float val)
    : mObj(obj)
    , mVal(val)
{
}

void StepGameSetObjectEnergy::OnStart()
{
    const float val0 = mObj->GetEnergy();
    const float delta = mVal - val0;

    mObj->SumEnergy(delta);

    SetDone();
}

} // namespace game
