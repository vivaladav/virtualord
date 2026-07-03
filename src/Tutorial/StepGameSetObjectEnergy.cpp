#include "Tutorial/StepGameSetObjectEnergy.h"

#include "GameObjects/GameObject.h"

namespace game
{

StepGameSetObjectEnergy::StepGameSetObjectEnergy(GameObject * obj, float val)
    : mVal(val)
{
    mObjs.emplace_back(obj);
}

StepGameSetObjectEnergy::StepGameSetObjectEnergy(const std::vector<GameObject *> && obj, float val)
    : mObjs(obj)
    , mVal(val)
{
}

void StepGameSetObjectEnergy::OnStart()
{
    TutorialStep::OnStart();

    for(GameObject * obj : mObjs)
    {
        const float val0 = obj->GetEnergy();
        const float delta = mVal - val0;

        obj->SumEnergy(delta);
    }

    SetDone();
}

} // namespace game
