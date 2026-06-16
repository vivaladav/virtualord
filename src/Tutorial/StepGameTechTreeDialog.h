#pragma once

#include "Tutorial/TutorialInfoStep.h"


namespace game
{

class FocusArea;
class GameHUD;

class StepGameTechTreeDialog : public TutorialInfoStep
{
public:
    StepGameTechTreeDialog(GameHUD * HUD);
    ~StepGameTechTreeDialog();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;

    GameHUD * mHUD = nullptr;
};

} // namespace game
