#include "Tutorial/StepGameSetObjectPerfectShot.h"

#include "GameObjects/GameObject.h"

namespace game
{

StepGameSetObjectPerfectShot::StepGameSetObjectPerfectShot(GameObject * obj, bool enabled)
    : mObj(obj)
    , mEnabled(enabled)
{
}

void StepGameSetObjectPerfectShot::OnStart()
{
    mObj->SetPerfectShot(mEnabled);

    SetDone();
}

} // namespace game
