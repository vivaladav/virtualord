#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class FocusArea;
class PanelClickFilter;
class PanelInfoTutorial;
class PanelPlanetActionConquer;

class StepPlanetMapConquerTerritoryStart : public TutorialStep
{
public:
    StepPlanetMapConquerTerritoryStart(PanelPlanetActionConquer * panel);
    ~StepPlanetMapConquerTerritoryStart();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;
};

} // namespace game
