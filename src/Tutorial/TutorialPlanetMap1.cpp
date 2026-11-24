#include "Tutorial/TutorialPlanetMap1.h"

#include "Screens/Screen.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepPlanetMapConquerTerritory.h"
#include "Tutorial/StepPlanetMapConquerTerritoryStart.h"
#include "Tutorial/StepPlanetMapExploreCancel.h"
#include "Tutorial/StepPlanetMapIntro.h"
#include "Tutorial/StepPlanetMapExploreTerritory.h"
#include "Tutorial/StepPlanetMapExploreTerritoryInfo.h"
#include "Tutorial/StepPlanetMapExploreTerritoryStart.h"
#include "Tutorial/StepPlanetMapExploreTerritorySuccess.h"
#include "Tutorial/StepPlanetMapNoInfo.h"
#include "Tutorial/StepPlanetMapNoResources.h"
#include "Tutorial/StepPlanetMapSelectTerritory.h"
#include "Tutorial/StepPlanetMapSendAI.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelPlanetActionExplore.h"

#include <sgl/sgui/AbstractButton.h>

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
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this] { return new StepPlanetMapSendAI(GetPanelActions()); });
    AddStep([this]
        {
            return new StepPlanetMapConquerTerritory(GetPanelActions(),
                                                     "TUT_PM_CONQUER_TERRITORY_2", true);
        });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this] { return new StepPlanetMapConquerTerritoryStart(GetPanelConquer()); });
}

} // namespace game
