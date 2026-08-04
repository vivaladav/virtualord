#include "Tutorial/StepDelay.h"

namespace game
{

void StepDelay::Update(float delta)
{
    mTimer -= delta;

    if(mTimer < 0.f)
        SetDone();
}

} // namespace game
