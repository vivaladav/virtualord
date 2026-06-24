#pragma once

#include "Tutorial/TutorialInfoStep.h"


namespace game
{

class FocusArea;
class GameHUD;

class StepGameSecondaryMissionGoal : public TutorialInfoStep
{
public:
    StepGameSecondaryMissionGoal(GameHUD * HUD, int goal);
    ~StepGameSecondaryMissionGoal();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;

    GameHUD * mHUD = nullptr;
};

} // namespace game
