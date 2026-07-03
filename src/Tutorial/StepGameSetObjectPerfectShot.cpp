#include "Tutorial/StepGameSetObjectPerfectShot.h"

#include "GameObjects/GameObject.h"

namespace game
{

StepGameSetObjectPerfectShot::StepGameSetObjectPerfectShot(GameObject * obj, bool enabled)
    : mEnabled(enabled)
{
}

StepGameSetObjectPerfectShot::StepGameSetObjectPerfectShot(const std::vector<GameObject *> && objs,
                                                           bool enabled)
    : mObjs(objs)
    , mEnabled(enabled)
{

}

void StepGameSetObjectPerfectShot::OnStart()
{
    TutorialStep::OnStart();

    for(GameObject * obj : mObjs)
        obj->SetPerfectShot(mEnabled);

    SetDone();
}

} // namespace game
