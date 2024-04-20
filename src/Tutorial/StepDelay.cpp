#include "Tutorial/StepDelay.h"

namespace game
{

StepDelay::StepDelay(float sec)
    : mTimer(sec)
{
}

void StepDelay::Update(float delta)
{
    mTimer -= delta;

    if(mTimer <= 0.f)
        SetDone();
}

} // namespace game
