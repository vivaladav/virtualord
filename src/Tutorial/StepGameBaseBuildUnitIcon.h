#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class Game;
class PanelObjectActions;

class StepGameBaseBuildUnitIcon : public TutorialInfoStep
{
public:
    StepGameBaseBuildUnitIcon(const Game * game, Screen * screen, PanelObjectActions * panel);
    ~StepGameBaseBuildUnitIcon();

private:
    FocusArea * mFocusArea = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
