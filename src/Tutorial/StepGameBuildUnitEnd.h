#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class GameHUD;
class FocusArea;

class StepGameBuildUnitEnd : public TutorialInfoStep
{
public:
    StepGameBuildUnitEnd(GameHUD * HUD);
    ~StepGameBuildUnitEnd();

    void OnStart() override;
    void OnEnd() override;

private:
    FocusArea * mFocusArea = nullptr;

    GameHUD * mHUD = nullptr;
    unsigned int mButtonActId;
};

} // namespace game
