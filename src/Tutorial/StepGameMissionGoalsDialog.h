#pragma once

#include "Tutorial/TutorialStep.h"


namespace game
{

class FocusArea;
class GameHUD;
class PanelClickFilter;
class PanelInfoTutorial;

class StepGameMissionGoalsDialog : public TutorialStep
{
public:
    StepGameMissionGoalsDialog(GameHUD * HUD);
    ~StepGameMissionGoalsDialog();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;

    GameHUD * mHUD = nullptr;
};

} // namespace game
