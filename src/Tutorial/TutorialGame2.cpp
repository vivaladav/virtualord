#include "Tutorial/TutorialGame2.h"

#include "Tutorial/StepDelay.h"
#include "Tutorial/TutorialConstants.h"

namespace
{

}

namespace game
{

TutorialGame2::TutorialGame2(Screen * screen)
    : TutorialGame(screen, TUTORIAL_MISSION_2)
{
    AddStep([] { return new StepDelay(1.f); });
}

TutorialGame2::~TutorialGame2()
{

}

} // namespace game
