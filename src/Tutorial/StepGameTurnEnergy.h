#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class FocusArea;
class GameHUD;
class PanelClickFilter;
class PanelInfoTutorial;

class StepGameTurnEnergy : public TutorialStep
{
public:
    StepGameTurnEnergy(const GameHUD * HUD);
    ~StepGameTurnEnergy();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;
};

} // namespace game
