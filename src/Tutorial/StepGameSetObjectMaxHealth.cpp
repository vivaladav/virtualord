#include "Tutorial/StepGameSetObjectMaxHealth.h"

#include "GameObjects/GameObject.h"

namespace game
{

StepGameSetObjectMaxHealth::StepGameSetObjectMaxHealth(GameObject * obj, float val)
    : mObj(obj)
    , mVal(val)
{
}

void StepGameSetObjectMaxHealth::OnStart()
{
    mObj->SetMaxHealth(mVal);

    SetDone();
}

} // namespace game
