#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class PlayerAI;

class StepAISetActive : public TutorialStep
{
public:
    StepAISetActive(PlayerAI * ai, bool active);

    void OnStart() override;

private:
    PlayerAI * mAI = nullptr;
    bool mActive;
};

} // namespace game
