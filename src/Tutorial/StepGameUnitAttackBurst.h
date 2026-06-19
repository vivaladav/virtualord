#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class FocusArea;
class PanelShotType;

class StepGameUnitAttackBurst : public TutorialInfoStep
{
public:
    StepGameUnitAttackBurst(PanelShotType * panel, const char * intro,
                            const sgl::core::Pointd2D & p0);
    ~StepGameUnitAttackBurst();

private:
    FocusArea * mFocusArea = nullptr;
    PanelShotType * mPanel = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
