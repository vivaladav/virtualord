#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class GameHUD;
class FocusArea;

class StepGameBaseBuildUnitEnd : public TutorialInfoStep
{
public:
    StepGameBaseBuildUnitEnd(GameHUD * HUD);
    ~StepGameBaseBuildUnitEnd();

    void OnStart() override;
    void OnEnd() override;

private:
    FocusArea * mFocusArea = nullptr;

    GameHUD * mHUD = nullptr;
    unsigned int mButtonActId;
};

} // namespace game
