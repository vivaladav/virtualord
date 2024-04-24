#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class FocusArea;
class PanelClickFilter;
class PanelInfoTutorial;
class PanelPlanetActionExplore;

class StepPlanetMapExploreTerritorySuccess : public TutorialStep
{
public:
    StepPlanetMapExploreTerritorySuccess(PanelPlanetActionExplore * panelExplore);
    ~StepPlanetMapExploreTerritorySuccess();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;
};

} // namespace game
