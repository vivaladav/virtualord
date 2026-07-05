#include "Tutorial/TutorialPlanetMap2.h"

#include "Game.h"
#include "Planet.h"
#include "Player.h"
#include "Screens/ScreenPlanetMap.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepGameSingleInfo.h"
#include "Tutorial/TutorialConstants.h"

#include <cassert>

namespace
{
const unsigned int mission0 = 0;
}

namespace game
{

TutorialPlanetMap2::TutorialPlanetMap2(Screen * screen)
    : Tutorial(TUTORIAL_PLANET_MAP_2)
    , mScreen(dynamic_cast<ScreenPlanetMap *>(screen))
{
    using namespace sgl;

    assert(mScreen != nullptr);

    const auto game = mScreen->GetGame();
    const auto planet = game->GetCurrentPlanet();
    const auto localPlayer = game->GetLocalPlayer();
    const PlayerFaction localFaction = localPlayer->GetFaction();

    AddStep([] { return new StepDelay(1.f); });
    AddStep([]
            {
                const core::Pointd2D p0(1100, 600);
                return new StepGameSingleInfo(p0, "TODO");
            });
}

} // namespace game
