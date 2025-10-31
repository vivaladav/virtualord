#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class FocusArea;
class PanelClickFilter;
class PanelInfoTutorial;
class PanelObjectActions;

class StepGameMissionGoalsIcon : public TutorialStep
{
public:
    StepGameMissionGoalsIcon(PanelObjectActions * panel);
    ~StepGameMissionGoalsIcon();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
