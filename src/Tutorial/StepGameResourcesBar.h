#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class GameHUD;

class StepGameResourcesBar : public TutorialInfoStep
{
public:
    StepGameResourcesBar(GameHUD * HUD);
    ~StepGameResourcesBar();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;
};

} // namespace game
