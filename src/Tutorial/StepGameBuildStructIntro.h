#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class FocusArea;
class Game;
class PanelObjectActions;

class StepGameBuildStructIntro : public TutorialInfoStep
{
public:
    StepGameBuildStructIntro(const Game * game, PanelObjectActions * panel, const char * textIntro,
                             const sgl::core::Pointd2D & p0);
    ~StepGameBuildStructIntro();

private:
    FocusArea * mFocusArea = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
