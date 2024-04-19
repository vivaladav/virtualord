#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class PanelInfoTutorial;

class StepGameIntro : public TutorialStep
{
public:
    StepGameIntro();

    void OnStart() override;
    void OnEnd() override;

private:
    PanelInfoTutorial * mInfo = nullptr;
};

} // namespace game
