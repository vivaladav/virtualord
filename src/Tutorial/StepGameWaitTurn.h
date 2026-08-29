#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class ScreenGame;

class StepGameWaitTurn : public TutorialStep
{
public:
    StepGameWaitTurn(const ScreenGame * screen);

    void Update(float delta) override;

private:
    const ScreenGame * mScreen = nullptr;
};

} // namespace game
