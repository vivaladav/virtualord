#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class PanelObjectActions;

class StepGameBaseBuildUnitStart : public TutorialInfoStep
{
public:
    StepGameBaseBuildUnitStart(PanelObjectActions * panel);
    ~StepGameBaseBuildUnitStart();

private:
    FocusArea * mFocusArea = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
