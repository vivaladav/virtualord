#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class PanelClickFilter;

class StepDelay : public TutorialStep
{
public:
    StepDelay(float sec);
    ~StepDelay();

    void OnStart() override;

    void Update(float delta) override;

private:
    PanelClickFilter * mClickFilter = nullptr;

    float mTimer = 0.f;
};

} // namespace game
