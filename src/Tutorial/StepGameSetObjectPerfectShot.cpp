#include "Tutorial/StepGameSetObjectPerfectShot.h"

#include "GameObjects/GameObject.h"

namespace game
{

StepGameSetObjectPerfectShot::StepGameSetObjectPerfectShot(GameObject * obj, bool enabled,
                                                           bool forceFatal)
    : mEnabled(enabled)
    , mForceFatal(forceFatal)
{
    mObjs.emplace_back(obj);
}

StepGameSetObjectPerfectShot::StepGameSetObjectPerfectShot(const std::vector<GameObject *> && objs,
                                                           bool enabled, bool forceFatal)
    : mObjs(objs)
    , mEnabled(enabled)
    , mForceFatal(forceFatal)
{
}

void StepGameSetObjectPerfectShot::OnStart()
{
    TutorialStep::OnStart();

    for(GameObject * obj : mObjs)
        obj->SetPerfectShotEnabled(mEnabled, mForceFatal);

    SetDone();
}

} // namespace game
