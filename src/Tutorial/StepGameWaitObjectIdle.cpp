#include "Tutorial/StepGameWaitObjectIdle.h"

#include "GameObjects/GameObject.h"

namespace game
{

StepGameWaitObjectIdle::StepGameWaitObjectIdle(const GameObject * obj)
    : mObj(obj)
{
}

void StepGameWaitObjectIdle::Update(float delta)
{
    if(mObj->GetCurrentAction() == IDLE)
        SetDone();
}

} // namespace game
