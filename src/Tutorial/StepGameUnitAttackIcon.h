#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class FocusArea;
class PanelObjectActions;

class StepGameUnitAttackIcon : public TutorialInfoStep
{
public:
    StepGameUnitAttackIcon(PanelObjectActions * panel, const sgl::core::Pointd2D & p0);
    ~StepGameUnitAttackIcon();

private:
    FocusArea * mFocusArea = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
