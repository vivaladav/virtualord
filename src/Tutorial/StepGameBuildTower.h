#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class GameHUD;
class FocusArea;

class StepGameBuildTower : public TutorialInfoStep
{
public:
    StepGameBuildTower(GameHUD * HUD);
    ~StepGameBuildTower();

    void OnStart() override;
    void OnEnd() override;

private:
    FocusArea * mFocusArea = nullptr;

    GameHUD * mHUD = nullptr;
    unsigned int mButtonCatId;
    unsigned int mButtonStructId;
    unsigned int mButtonBuildId;
};

} // namespace game
