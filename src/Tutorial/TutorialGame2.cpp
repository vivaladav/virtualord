#include "Tutorial/TutorialGame2.h"

#include "CameraMapController.h"
#include "Game.h"
#include "Player.h"
#include "GameObjects/Base.h"
#include "Screens/ScreenGame.h"
#include "Tutorial/StepAISetActive.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepGameIntro2.h"
#include "Tutorial/StepGameDisableCamera.h"
#include "Tutorial/StepGameBaseBuildUnitStart.h"
#include "Tutorial/StepGameBaseBuildUnitEnd.h"
#include "Tutorial/StepGameConquerStructChoice.h"
#include "Tutorial/StepGameSelectBase.h"
#include "Tutorial/StepGameUnit.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/GameHUD.h"

namespace
{
using namespace game;

constexpr unsigned int indUnit1 = 0;

const Cell2D cellEneGen1(6, 15);
const Cell2D cellMatGen1(15, 7);

}

namespace game
{

TutorialGame2::TutorialGame2(Screen * screen)
    : TutorialGame(screen, TUTORIAL_MISSION_2)
{
    auto gs = GetScreen();
    auto game = gs->GetGame();
    auto hud = gs->GetHUD();
    auto isoMap = GetIsoMap();

    const Player * local = game->GetPlayerByIndex(0);
    Player * playerAI = game->GetPlayerByIndex(1);

    auto localBase = local->GetBase();
    auto panelActions = hud->GetPanelObjectActions();

    // ===== SETUP =====
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    // make AI idle for now
    AddStep([playerAI] { return new StepAISetActive(playerAI->GetAI(), false); });
    AddStep([] { return new StepDelay(1.f); });

    // ===== PART 1 =====
    // INTRO
    AddStep([] { return new StepGameIntro2; });
    AddStep([] { return new StepDelay(0.3f); });
    // BUILD FIRST UNIT
    AddStep([localBase, game] { return new StepGameSelectBase(game, localBase); });
    AddStep([panelActions] { return new StepGameBaseBuildUnitStart(panelActions); });
    AddStep([hud] { return new StepGameBaseBuildUnitEnd(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([localBase] { return new StepDelay(localBase->GetTimeBuildUnit()); });
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indUnit1);
            return new StepGameUnit(game, isoMap, unit);
        });
    // CONQUEST FIRST GENERATOR
    AddStep([this, local, game, isoMap]
        {
            const GameObject * unit = local->GetSelectedObject();
            const GameObject * gen1 = GetObjectInCell(cellEneGen1);
            const GameObject * gen2 = GetObjectInCell(cellMatGen1);

            return new StepGameConquerStructChoice(game, unit, gen1, gen2, isoMap);
        });
}

} // namespace game
