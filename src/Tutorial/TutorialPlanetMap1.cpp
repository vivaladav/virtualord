#include "Tutorial/TutorialPlanetMap1.h"

#include "Screens/Screen.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepPlanetMapConquerTerritory.h"
#include "Tutorial/StepPlanetMapConquerTerritoryStart.h"
#include "Tutorial/StepPlanetMapIntro.h"
#include "Tutorial/StepPlanetMapExploreTerritory.h"
#include "Tutorial/StepPlanetMapExploreTerritoryInfo.h"
#include "Tutorial/StepPlanetMapExploreTerritoryStart.h"
#include "Tutorial/StepPlanetMapExploreTerritorySuccess.h"
#include "Tutorial/StepPlanetMapNoInfo.h"
#include "Tutorial/StepPlanetMapSelectTerritory.h"
#include "Tutorial/StepPlanetMapSendAI.h"
#include "Tutorial/TutorialConstants.h"

namespace game
{

TutorialPlanetMap1::TutorialPlanetMap1(Screen * screen)
    : TutorialPlanetMap(screen, TUTORIAL_PLANET_MAP_1)
{
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([]
        {
            return new StepPlanetMapIntro("TUT_PM_INTRO_3");
        });
    AddStep([this]
        {
            const int missionId = 2;
            return new StepPlanetMapSelectTerritory(GetPlanetMap(), missionId, true);
        });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this] { return new StepPlanetMapNoInfo(GetPanelInfo(), GetPanelResources()); });
    AddStep([this] { return new StepPlanetMapExploreTerritory(GetPanelActions()); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([] { return new StepPlanetMapExploreTerritoryInfo; });
    AddStep([this] { return new StepPlanetMapExploreTerritoryStart(GetPanelExplore()); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this]
        {
            return new StepPlanetMapExploreTerritorySuccess(GetPanelExplore(), GetPanelInfo(),
                                                            GetPanelResources());
        });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this] { return new StepPlanetMapSendAI(GetPanelActions()); });
    AddStep([this] { return new StepPlanetMapConquerTerritory(GetPanelActions()); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this] { return new StepPlanetMapConquerTerritoryStart(GetPanelConquer()); });
}

} // namespace game
