#pragma once

#include "Tutorial/TutorialStep.h"

namespace sgl
{
    namespace sgui { class AbstractButton; }
}

namespace game
{

class FocusArea;
class PanelClickFilter;
class PanelInfoTutorial;
class PlanetMap;

class StepPlanetMapSelectTerritory : public TutorialStep
{
public:
    StepPlanetMapSelectTerritory(PlanetMap * planet);
    ~StepPlanetMapSelectTerritory();

    void OnStart() override;

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;

    sgl::sgui::AbstractButton * mTerritory = nullptr;
    bool mCheckTerritorySelected = false;
};

} // namespace game
