#include "Tutorial/TutorialGame2.h"

#include "Screens/ScreenGame.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/TutorialConstants.h"

#include <cassert>

namespace
{

}

namespace game
{

TutorialGame2::TutorialGame2(Screen * screen)
    : Tutorial(TUTORIAL_MISSION_2)
    , mScreen(dynamic_cast<ScreenGame *>(screen))
{
    assert(mScreen);

    AddStep([] { return new StepDelay(1.f); });
}

TutorialGame2::~TutorialGame2()
{

}

} // namespace game
