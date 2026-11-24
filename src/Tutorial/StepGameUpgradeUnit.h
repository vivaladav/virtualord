#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class GameHUD;
class FocusArea;
class Player;

class StepGameUpgradeUnit : public TutorialInfoStep
{
public:
    StepGameUpgradeUnit(const Player * p, GameHUD * HUD, bool showIntro);
    ~StepGameUpgradeUnit();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;

    GameHUD * mHUD = nullptr;
    unsigned int mButtonIncId;
    unsigned int mButtonUpgId;
};

} // namespace game
