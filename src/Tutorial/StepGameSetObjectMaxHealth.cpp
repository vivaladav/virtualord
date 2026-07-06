#include "Tutorial/StepGameSetObjectMaxHealth.h"

#include "GameObjects/GameObject.h"

namespace game
{

StepGameSetObjectMaxHealth::StepGameSetObjectMaxHealth(GameObject * obj, float val)
    : mVal(val)
{
    mObjs.emplace_back(obj);
}

StepGameSetObjectMaxHealth::StepGameSetObjectMaxHealth(const std::vector<GameObject *> && objs,
                                                       float val)
    : mObjs(objs)
    , mVal(val)
{
}

void StepGameSetObjectMaxHealth::OnStart()
{
    TutorialStep::OnStart();

    for(GameObject * obj : mObjs)
        obj->SetMaxHealth(mVal);

    SetDone();
}

} // namespace game
