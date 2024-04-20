#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class StepDelay : public TutorialStep
{
public:
    StepDelay(float sec);

    void Update(float delta) override;

private:
    float mTimer = 0.f;
};

} // namespace game
