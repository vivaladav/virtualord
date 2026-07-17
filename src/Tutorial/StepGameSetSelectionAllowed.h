#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class ScreenGame;

class StepGameSetSelectionAllowed : public TutorialStep
{
public:
    StepGameSetSelectionAllowed(ScreenGame * screen, bool allowed);

    void OnStart() override;

private:
    ScreenGame * mScreen = nullptr;

    bool mAllowed;
};

} // namespace game
