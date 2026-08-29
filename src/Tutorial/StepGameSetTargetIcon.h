#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class FocusArea;
class Game;
class PanelObjectActions;

class StepGameSetTargetIcon : public TutorialInfoStep
{
public:
    StepGameSetTargetIcon(const Game * game, Screen * screen, PanelObjectActions * panel,
                          const sgl::core::Pointd2D & p0);
    ~StepGameSetTargetIcon();

private:
    FocusArea * mFocusArea = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
