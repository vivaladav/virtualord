#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class PanelObjectActions;

class StepGameWallBuildIcon : public TutorialInfoStep
{
public:
    StepGameWallBuildIcon(PanelObjectActions * panel);
    ~StepGameWallBuildIcon();

private:
    FocusArea * mFocusArea = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
