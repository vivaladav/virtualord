#include "Tutorial/TutorialPlanetMap2.h"

#include "Screens/Screen.h"
#include "Tutorial/StepDelay.h"
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
}

} // namespace game
