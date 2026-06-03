#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class FocusArea;
class PanelObjectActions;

class StepGameBuildTowerIntro : public TutorialInfoStep
{
public:
    StepGameBuildTowerIntro(PanelObjectActions * panel, const char * textIntro,
                            const sgl::core::Pointd2D & p0);
    ~StepGameBuildTowerIntro();

private:
    FocusArea * mFocusArea = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
