#include "Tutorial/StepGameSetObjectFatalHit.h"

#include "GameObjects/GameObject.h"

namespace game
{

StepGameSetObjectFatalHit::StepGameSetObjectFatalHit(GameObject * obj, bool enabled)
    : mObj(obj)
    , mEnabled(enabled)
{
}

void StepGameSetObjectFatalHit::OnStart()
{
    TutorialStep::OnStart();

    mObj->SetFatalHit(mEnabled);

    SetDone();
}

} // namespace game
