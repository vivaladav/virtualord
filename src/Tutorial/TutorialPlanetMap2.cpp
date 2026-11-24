#include "Tutorial/TutorialPlanetMap2.h"

#include "Screens/Screen.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepPlanetMapConquerTerritory.h"
#include "Tutorial/StepPlanetMapConquerTerritoryStart.h"
#include "Tutorial/StepPlanetMapExploreCancel.h"
#include "Tutorial/StepPlanetMapExploreTerritory.h"
#include "Tutorial/StepPlanetMapExploreTerritoryStart.h"
#include "Tutorial/StepPlanetMapExploreTerritorySuccess.h"
#include "Tutorial/StepPlanetMapIntro.h"
#include "Tutorial/StepPlanetMapNoResources.h"
#include "Tutorial/StepPlanetMapSelectTerritory.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelPlanetActionExplore.h"

#include <sgl/sgui/AbstractButton.h>

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
    AddStep([this]
    {
        TutorialStep * step = nullptr;
        auto panel = GetPanelExplore();

        if(panel->GetButtonOk()->IsEnabled())
            step = new StepPlanetMapExploreTerritoryStart(panel);
        else
            step = new StepPlanetMapNoResources;

        return step;
    });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this]
    {
        TutorialStep * step = nullptr;
        auto panel = GetPanelExplore();

        if(panel->GetButtonOk()->IsEnabled())
            step = new StepPlanetMapExploreTerritorySuccess(panel, GetPanelInfo(),
                                                            GetPanelResources());
        else
            step = new StepPlanetMapExploreCancel(panel);

        return step;
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
