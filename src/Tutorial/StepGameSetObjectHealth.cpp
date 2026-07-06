#include "Tutorial/StepGameSetObjectHealth.h"

#include "GameObjects/GameObject.h"

namespace game
{

StepGameSetObjectHealth::StepGameSetObjectHealth(GameObject * obj, float val)
    : mVal(val)
{
    mObjs.emplace_back(obj);
}

StepGameSetObjectHealth::StepGameSetObjectHealth(const std::vector<GameObject *> && objs, float val)
    : mObjs(objs)
    , mVal(val)
{
}

void StepGameSetObjectHealth::OnStart()
{
    TutorialStep::OnStart();

    for(GameObject * obj : mObjs)
    {
        const float val0 = obj->GetHealth();
        const float delta = mVal - val0;

        obj->SumHealth(delta);
    }

    SetDone();
}

} // namespace game
