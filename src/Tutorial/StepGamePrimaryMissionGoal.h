#pragma once

#include "Tutorial/TutorialInfoStep.h"


namespace game
{

class FocusArea;
class GameHUD;

class StepGamePrimaryMissionGoal : public TutorialInfoStep
{
public:
    StepGamePrimaryMissionGoal(GameHUD * HUD);
    ~StepGamePrimaryMissionGoal();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;

    GameHUD * mHUD = nullptr;
};

} // namespace game
