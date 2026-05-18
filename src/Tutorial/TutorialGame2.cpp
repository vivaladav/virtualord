#include "Tutorial/TutorialGame2.h"

#include "CameraMapController.h"
#include "Game.h"
#include "Player.h"
#include "Screens/ScreenGame.h"
#include "Tutorial/StepAISetActive.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepGameIntro2.h"
#include "Tutorial/StepGameDisableCamera.h"
#include "Tutorial/TutorialConstants.h"

namespace
{

}

namespace game
{

TutorialGame2::TutorialGame2(Screen * screen)
    : TutorialGame(screen, TUTORIAL_MISSION_2)
{
    auto gs = GetScreen();
    auto game = gs->GetGame();

    const Player * local = game->GetPlayerByIndex(0);
    Player * playerAI = game->GetPlayerByIndex(1);

    // ===== SETUP =====
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    // make AI idle for now
    AddStep([playerAI] { return new StepAISetActive(playerAI->GetAI(), false); });
    AddStep([] { return new StepDelay(1.f); });

    // ===== PART 1 =====
    // INTRO
    AddStep([] { return new StepGameIntro2; });
    AddStep([] { return new StepDelay(0.3f); });
}

} // namespace game
