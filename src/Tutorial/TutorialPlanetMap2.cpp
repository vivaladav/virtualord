#include "Tutorial/TutorialPlanetMap2.h"

#include "Screens/Screen.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepPlanetMapConquerTerritory.h"
#include "Tutorial/StepPlanetMapConquerTerritoryStart.h"
#include "Tutorial/StepPlanetMapExploreTerritory.h"
#include "Tutorial/StepPlanetMapExploreTerritoryStart.h"
#include "Tutorial/StepPlanetMapExploreTerritorySuccess.h"
#include "Tutorial/StepPlanetMapIntro.h"
#include "Tutorial/StepPlanetMapSelectTerritory.h"
#include "Tutorial/TutorialConstants.h"

namespace
{
const unsigned int mission0 = 0;
}

namespace game
{

TutorialPlanetMap2::TutorialPlanetMap2(Screen * screen)
    : TutorialPlanetMap(screen, TUTORIAL_PLANET_MAP_2)
{
    using namespace sgl;

    // INTRO
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([]
            {
                return new StepPlanetMapIntro("TUT_PM_INTRO_4");
            });
    // SELECT MAIN MISSION
    AddStep([this]
            {
                const int missionId = 4;
                return new StepPlanetMapSelectTerritory(GetPlanetMap(), missionId, false);
            });
    // EXPLORE MAIN TERRITORY
    AddStep([this] { return new StepPlanetMapExploreTerritory(GetPanelActions()); });
    AddStep([this] { return new StepPlanetMapExploreTerritoryStart(GetPanelExplore()); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this]
            {
                return new StepPlanetMapExploreTerritorySuccess(GetPanelExplore(), GetPanelInfo(),
                                                                GetPanelResources());
            });
    // CONQUER TERRITORY
    AddStep([this]
            {
                return new StepPlanetMapConquerTerritory(GetPanelActions(),
                                                         "TUT_PM_CONQUER_TERRITORY_3", false);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this] { return new StepPlanetMapConquerTerritoryStart(GetPanelConquer()); });
}

} // namespace game
