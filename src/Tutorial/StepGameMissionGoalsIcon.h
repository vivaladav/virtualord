#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class PanelObjectActions;

class StepGameMissionGoalsIcon : public TutorialInfoStep
{
public:
    StepGameMissionGoalsIcon(PanelObjectActions * panel, bool showIntro);
    ~StepGameMissionGoalsIcon();

private:
    FocusArea * mFocusArea = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
