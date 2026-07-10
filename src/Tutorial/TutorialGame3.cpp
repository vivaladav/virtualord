#include "Tutorial/TutorialGame3.h"

#include "CameraMapController.h"
#include "Game.h"
#include "GameMap.h"
#include "IsoObject.h"
#include "Player.h"
#include "AI/PlayerAI.h"
#include "GameObjects/Base.h"
#include "GameObjects/ObjectData.h"
#include "GameObjects/Unit.h"
#include "Indicators/OverlayCellConquest.h"
#include "Indicators/OverlayWall.h"
#include "Screens/ScreenGame.h"
#include "Tutorial/StepAISetActive.h"
#include "Tutorial/StepGameBuildUnitEnd.h"
#include "Tutorial/StepGameBuildUnitStart.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepGameDisableCamera.h"
#include "Tutorial/StepGameIntro3.h"
#include "Tutorial/StepGameSelectObject.h"
#include "Tutorial/StepGameSingleInfo.h"
#include "Tutorial/StepGameUnit.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/GameHUD.h"
#include "Widgets/PanelObjectActions.h"

namespace
{
using namespace game;

constexpr unsigned int indWorker1 = 0;

}

namespace game
{

TutorialGame3::TutorialGame3(Screen * screen)
    : TutorialGame(screen, TUTORIAL_MISSION_3)
{
    using namespace sgl;

    auto gs = GetScreen();
    auto game = gs->GetGame();
    auto hud = gs->GetHUD();
    auto isoMap = GetIsoMap();

    const Player * local = game->GetPlayerByIndex(0);
    Player * playerAI = game->GetPlayerByIndex(1);

    auto panelActions = hud->GetPanelObjectActions();
    auto panelTurn = hud->GetPanelTurnControl();
    auto localBase = local->GetBase();

    // ===== SETUP =====
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    // make AI idle for now
    AddStep([playerAI] { return new StepAISetActive(playerAI->GetAI(), false); });
    AddStep([] { return new StepDelay(0.5f); });

    // ===== PART 1 =====
    // INTRO
    AddStep([] { return new StepGameIntro3; });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD FIRST UNIT
    AddStep([localBase, game, isoMap]
            {
                const core::Pointd2D p0(500, 200);
                return new StepGameSelectObject(game, isoMap, localBase, "TUT_GAME_BASE_4", p0);
            });
    AddStep([panelActions]
            {
                return new StepGameBuildUnitStart(panelActions, PanelObjectActions::BTN_BUILD_UNIT_BASE);
            });
    AddStep([hud] { return new StepGameBuildUnitEnd(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([localBase] { return new StepDelay(localBase->GetTimeBuildUnit()); });
    // SELECT WORKER 1
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(1300, 450);
                return new StepGameUnit(game, isoMap, unit, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
}

TutorialGame3::~TutorialGame3()
{

}

} // namespace game
