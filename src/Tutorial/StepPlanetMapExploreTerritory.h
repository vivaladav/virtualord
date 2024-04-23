#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class FocusArea;
class PanelClickFilter;
class PanelInfoTutorial;
class PanelPlanetActions;

class StepPlanetMapExploreTerritory : public TutorialStep
{
public:
    StepPlanetMapExploreTerritory(PanelPlanetActions * panelActions);
    ~StepPlanetMapExploreTerritory();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;
};

} // namespace game
