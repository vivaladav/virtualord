#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class FocusArea;
class PanelClickFilter;
class PanelInfoTutorial;
class PanelPlanetActions;

class StepPlanetMapConquerTerritory : public TutorialStep
{
public:
    StepPlanetMapConquerTerritory(PanelPlanetActions * panelActions);
    ~StepPlanetMapConquerTerritory();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;
};

} // namespace game
