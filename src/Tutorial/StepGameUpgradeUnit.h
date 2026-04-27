#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class GameHUD;
class FocusArea;

class StepGameUpgradeUnit : public TutorialInfoStep
{
public:
    StepGameUpgradeUnit(GameHUD * HUD, bool showIntro);
    ~StepGameUpgradeUnit();

    void OnStart() override;
    void OnEnd() override;

private:
    FocusArea * mFocusArea = nullptr;

    GameHUD * mHUD = nullptr;
    unsigned int mButtonIncId;
    unsigned int mButtonUpgId;
};

} // namespace game
