#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class Game;
class PanelObjectActions;

class StepGameWallBuildIcon : public TutorialInfoStep
{
public:
    StepGameWallBuildIcon(const Game * game, PanelObjectActions * panel);
    ~StepGameWallBuildIcon();

private:
    FocusArea * mFocusArea = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
