#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class GameHUD;
class FocusArea;

class StepGameBaseBuildUnit : public TutorialInfoStep
{
public:
    StepGameBaseBuildUnit(GameHUD * HUD);
    ~StepGameBaseBuildUnit();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;

    GameHUD * mHUD = nullptr;
    unsigned int mButtonActId;
};

} // namespace game
