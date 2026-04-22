#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class PanelObjectActions;

class StepGameBuildTowerIntro : public TutorialInfoStep
{
public:
    StepGameBuildTowerIntro(PanelObjectActions * panel);
    ~StepGameBuildTowerIntro();

private:
    FocusArea * mFocusArea = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
