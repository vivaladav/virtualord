#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class GameHUD;
class FocusArea;

class StepGameUpgradeUnitFree : public TutorialInfoStep
{
public:
    StepGameUpgradeUnitFree(GameHUD * HUD);
    ~StepGameUpgradeUnitFree();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;

    GameHUD * mHUD = nullptr;
};

} // namespace game
