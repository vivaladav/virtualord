#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class PanelClickFilter;
class PanelInfoTutorial;

class StepPlanetMapExploreTerritoryInfo : public TutorialStep
{
public:
    StepPlanetMapExploreTerritoryInfo();
    ~StepPlanetMapExploreTerritoryInfo();

    void OnStart() override;

private:
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;
};

} // namespace game
