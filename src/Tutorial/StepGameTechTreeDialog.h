#pragma once

#include "Tutorial/TutorialInfoStep.h"


namespace game
{

class FocusArea;
class GameHUD;

enum TechUpgradeId : unsigned int;

class StepGameTechTreeDialog : public TutorialInfoStep
{
public:
    StepGameTechTreeDialog(GameHUD * HUD, TechUpgradeId upgradeID, bool showIntro);
    ~StepGameTechTreeDialog();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;

    GameHUD * mHUD = nullptr;
};

} // namespace game
