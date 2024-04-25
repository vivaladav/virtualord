#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class FocusArea;
class PanelClickFilter;
class PanelInfoTutorial;
class PanelPlanetActions;

class StepPlanetMapSendAI : public TutorialStep
{
public:
    StepPlanetMapSendAI(PanelPlanetActions * panel);
    ~StepPlanetMapSendAI();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;
};

} // namespace game
