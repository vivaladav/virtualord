#include "Tutorial/StepGameSetObjectAttackMode.h"

#include "GameObjects/GameObject.h"

namespace game
{

StepGameSetObjectAttackMode::StepGameSetObjectAttackMode(GameObject * obj, AttackMode mode)
    : mMode(mode)
{
    mObjs.emplace_back(obj);
}

StepGameSetObjectAttackMode::StepGameSetObjectAttackMode(const std::vector<GameObject *> && objs,
                                                         AttackMode mode)
    : mObjs(objs)
    , mMode(mode)
{
}

void StepGameSetObjectAttackMode::OnStart()
{
    TutorialStep::OnStart();

    for(GameObject * obj : mObjs)
        obj->SetAttackMode(mMode);

    SetDone();
}

} // namespace game
