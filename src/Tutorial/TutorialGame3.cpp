#include "Tutorial/TutorialGame3.h"

#include "CameraMapController.h"
#include "Game.h"
#include "GameMap.h"
#include "IsoObject.h"
#include "Player.h"
#include "AI/PlayerAI.h"
#include "GameObjects/Base.h"
#include "GameObjects/LootBox.h"
#include "GameObjects/ObjectData.h"
#include "GameObjects/Unit.h"
#include "Indicators/OverlayCellConquest.h"
#include "Indicators/OverlayWall.h"
#include "Screens/ScreenGame.h"
#include "Tutorial/StepAISetActive.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepGameBackToBase.h"
#include "Tutorial/StepGameBuildStructIntro.h"
#include "Tutorial/StepGameBuildStructure.h"
#include "Tutorial/StepGameBuildTowerEnd.h"
#include "Tutorial/StepGameBuildUnitEnd.h"
#include "Tutorial/StepGameBuildUnitStart.h"
#include "Tutorial/StepGameConnectStructIntro.h"
#include "Tutorial/StepGameConquerCellsEnd.h"
#include "Tutorial/StepGameConquerCellsSimple.h"
#include "Tutorial/StepGameConquerMaterialGenIntro.h"
#include "Tutorial/StepGameConquerStructSimple.h"
#include "Tutorial/StepGameDisableCamera.h"
#include "Tutorial/StepGameEndTurnSimple.h"
#include "Tutorial/StepGameIntro3.h"
#include "Tutorial/StepGameMoveUnitSimple.h"
#include "Tutorial/StepGameMoveUnitToCorner.h"
#include "Tutorial/StepGameOpenLootbox.h"
#include "Tutorial/StepGameSelectObject.h"
#include "Tutorial/StepGameSetLootboxPrize.h"
#include "Tutorial/StepGameSetSelectionActiveAction.h"
#include "Tutorial/StepGameSingleInfo.h"
#include "Tutorial/StepGameUnitConquerCellsIcon.h"
#include "Tutorial/StepGameUnit.h"
#include "Tutorial/StepGameWaitTurn.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/GameHUD.h"
#include "Widgets/PanelObjectActions.h"

namespace
{
using namespace game;

constexpr unsigned int indWorker1 = 0;
constexpr unsigned int indWorker2 = 1;

constexpr int catResources = 2;

constexpr int structDefTower = 1;
constexpr int structMatExtr = 1;

const Cell2D cellLootbox1(74, 63);
const Cell2D cellMatGen1(66, 62);

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

    const Player * local = game->GetActivePlayerByIndex(0);
    Player * playerAI = game->GetActivePlayerByIndex(1);

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
    // BUILD SOLAR PANEL 1
    AddStep([panelActions]
        {
           const core::Pointd2D p0(1100, 300);
            return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_RES_GEN", p0);
        });
    AddStep([hud]
        {
            const int indStruct = 0;
            return new StepGameBuildStructure(hud, "TUT_GAME_CAT_RES_GEN", nullptr,
                                              catResources, indStruct);
        });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(1000, 200);
            const Cell2D target(74, 78);
            return new StepGameBuildTowerEnd(isoMap, unit, target, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD SOLAR PANEL 2
    AddStep([panelActions]
        {
            const core::Pointd2D p0(1100, 250);
            return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_MORE", p0);
        });
    AddStep([hud]
        {
            const int indStruct = 0;
            return new StepGameBuildStructure(hud, "TUT_GAME_CAT_RES_GEN", nullptr,
                                              catResources, indStruct);
        });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(1000, 200);
            const Cell2D target(74, 77);
            return new StepGameBuildTowerEnd(isoMap, unit, target, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT SOLAR PANELS TO BASE
    AddStep([]
            {
                const core::Pointd2D p0(1100, 350);
                return new StepGameConnectStructIntro(p0);
            });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(1100, 400);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(75, 77);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(1100, 300);
                const Cell2D cellEnd(76, 77);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // BUILD MATERIAL EXTRACTOR
    AddStep([panelActions]
        {
            const core::Pointd2D p0(1100, 250);
            return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_MORE", p0);
        });
    AddStep([hud]
        {
            return new StepGameBuildStructure(hud, "TUT_GAME_CAT_RES_GEN", "TUT_GAME_BUILD_MAT_EXTR",
                                              catResources, structMatExtr);
        });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(1000, 200);
            const Cell2D target(74, 74);
            return new StepGameBuildTowerEnd(isoMap, unit, target, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT MATERIAL EXTRACTOR TO BASE
    AddStep([]
        {
            const core::Pointd2D p0(1100, 350);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1100, 400);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(74, 75);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(1100, 300);
            const Cell2D cellEnd(74, 76);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // MOVE WORKER 1 TO LOOTBOX
    AddStep([this, local, isoMap]
        {
            const auto unit = local->GetUnit(indWorker1);
            const Cell2D target(74, 66);
            const core::Pointd2D p0(1000, 150);
            return new StepGameMoveUnitToCorner(unit, isoMap, target, p0);
        });
    // OPEN LOOTBOX
    AddStep([this]
        {
            auto lootbox = static_cast<LootBox *>(GetObjectInCell(cellLootbox1));
            const unsigned int prizeType = LootBox::LB_MATERIAL;
            const int prizeQuantity = 200;
            return new StepGameSetLootboxPrize(lootbox, prizeType, prizeQuantity);
        });
    AddStep([this, local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker1);
            const GameObject * lootbox = GetObjectInCell(cellLootbox1);
            const core::Pointd2D p0(500, 150);

            return new StepGameOpenLootbox(game, unit, lootbox, isoMap, p0);
        });
    AddStep([local]
        {
            const auto unit = local->GetUnit(indWorker1);
            return new StepDelay(unit->GetTimeOpenLootbox());
        });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // MOVE TO MATERIAL GENERATOR
    AddStep([]
        {
            const core::Pointd2D p0(600, 250);
            return new StepGameSingleInfo(p0, "TUT_GAME_CONT_EXPL");
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const Cell2D target(68, 62);
            const core::Pointd2D p0(450, 200);
            return new StepGameMoveUnitSimple(game, unit, isoMap, target, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER MATERIAL GENERATOR WITH SOLDIER
    AddStep([this, local]
        {
            const GameObject * gen = GetObjectInCell(cellMatGen1);
            const core::Pointd2D p0(1250, 350);
            return new StepGameConquerMaterialGenIntro(gen, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const GameObject * gen = GetObjectInCell(cellMatGen1);
            const core::Pointd2D p0(400, 200);
            return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, IDLE); });
    // MOVE VIEW BACK TO BASE
    AddStep([panelTurn, game]
        {
            const core::Pointd2D p0(50, 600);
            return new StepGameBackToBase(panelTurn, "TUT_GAME_BACK_TO_BASE_1", p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD SECOND WORKER UNIT
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
}

TutorialGame3::~TutorialGame3()
{

}

} // namespace game
