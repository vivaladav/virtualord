#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace sgl
{
    namespace sgui { class AbstractButton; }
}

namespace game
{

class FocusArea;
class PlanetMap;

class StepPlanetMapSelectTerritory : public TutorialInfoStep
{
public:
    StepPlanetMapSelectTerritory(PlanetMap * planet, unsigned int missionInd, bool intro);
    ~StepPlanetMapSelectTerritory();

    void OnStart() override;

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;
    sgl::sgui::AbstractButton * mTerritory = nullptr;

    bool mCheckTerritorySelected = false;
};

} // namespace game
