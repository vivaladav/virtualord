#include "Tutorial/StepGameSetObjectFatalHit.h"

#include "GameObjects/GameObject.h"

namespace game
{

StepGameSetObjectFatalHit::StepGameSetObjectFatalHit(GameObject * obj, bool enabled)
    : mEnabled(enabled)
{
    mObjs.emplace_back(obj);
}

StepGameSetObjectFatalHit::StepGameSetObjectFatalHit(const std::vector<GameObject *> && objs,
                                                     bool enabled)
    : mObjs(objs)
    , mEnabled(enabled)
{
}

void StepGameSetObjectFatalHit::OnStart()
{
    TutorialStep::OnStart();

    for(GameObject * obj : mObjs)
        obj->SetFatalHit(mEnabled);

    SetDone();
}

} // namespace game
