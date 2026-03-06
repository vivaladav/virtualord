#include "Tutorial/TutorialPlanetMap.h"

#include "Game.h"
#include "MapsRegistry.h"
#include "Player.h"
#include "Screens/ScreenPlanetMap.h"
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

#include <cassert>

namespace
{
const unsigned int mission0 = 0;
}

namespace game
{

TutorialPlanetMap::TutorialPlanetMap(Screen * screen)
    : Tutorial(TUTORIAL_PLANET_MAP)
    , mScreen(dynamic_cast<ScreenPlanetMap *>(screen))
{
    assert(mScreen != nullptr);

    const auto game = mScreen->GetGame();
    const auto mr = game->GetMapsRegistry();
    const auto localPlayer = game->GetLocalPlayer();
    const PlayerFaction localFaction = localPlayer->GetFaction();

    AddStep([] { return new StepDelay(1.f); });
    AddStep([mr, localFaction]
        {
            const bool won = mr->GetMapOccupier(PLANET_1, mission0) == localFaction;
            return new StepPlanetMapIntro(won);
        });
    AddStep([this, mr, localFaction]
        {
            const bool won = mr->GetMapOccupier(PLANET_1, mission0) == localFaction;
            return new StepPlanetMapSelectTerritory(mScreen->mPlanet, won);
        });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this] { return new StepPlanetMapNoInfo(mScreen->mPanelInfo, mScreen->mPanelResources); });
    AddStep([this] { return new StepPlanetMapExploreTerritory(mScreen->mPanelActions); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([] { return new StepPlanetMapExploreTerritoryInfo; });
    AddStep([this] { return new StepPlanetMapExploreTerritoryStart(mScreen->mPanelExplore); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this]
            {
                return new StepPlanetMapExploreTerritorySuccess(mScreen->mPanelExplore,
                                                                mScreen->mPanelInfo,
                                                                mScreen->mPanelResources);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this] { return new StepPlanetMapSendAI(mScreen->mPanelActions); });
    AddStep([this] { return new StepPlanetMapConquerTerritory(mScreen->mPanelActions); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this] { return new StepPlanetMapConquerTerritoryStart(mScreen->mPanelConquer); });
}

} // namespace game
