#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class Game;
class PanelObjectActions;

class StepGameBuildUnitStart : public TutorialInfoStep
{
public:
    StepGameBuildUnitStart(const Game * game, PanelObjectActions * panel, unsigned int buttonId);
    ~StepGameBuildUnitStart();

private:
    FocusArea * mFocusArea = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mBtnId;
    unsigned int mClickId = 0;
};

} // namespace game
