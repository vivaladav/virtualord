#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class FocusArea;
class PanelObjectActions;

class StepGameSetupResearchIcon : public TutorialInfoStep
{
public:
    StepGameSetupResearchIcon(PanelObjectActions * panel, const sgl::core::Pointd2D & p0);
    ~StepGameSetupResearchIcon();

private:
    FocusArea * mFocusArea = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
