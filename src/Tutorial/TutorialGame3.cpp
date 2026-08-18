#include "Tutorial/TutorialGame3.h"

#include "CameraMapController.h"
#include "Game.h"
#include "GameMap.h"
#include "Player.h"
#include "AI/PlayerAI.h"
#include "GameObjects/Base.h"
#include "GameObjects/CityGroup.h"
#include "GameObjects/LootBox.h"
#include "GameObjects/ObjectData.h"
#include "GameObjects/Unit.h"
#include "Indicators/OverlayCellConquest.h"
#include "Indicators/OverlayWall.h"
#include "Screens/ScreenGame.h"
#include "Tutorial/StepAISetActive.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepGameAddEnemy.h"
#include "Tutorial/StepGameBackToBase.h"
#include "Tutorial/StepGameBuildStructIntro.h"
#include "Tutorial/StepGameBuildStructure.h"
#include "Tutorial/StepGameBuildTowerEnd.h"
#include "Tutorial/StepGameBuildUnitEnd.h"
#include "Tutorial/StepGameBuildUnitStart.h"
#include "Tutorial/StepGameWallBuildIntro.h"
#include "Tutorial/StepGameCityIntro.h"
#include "Tutorial/StepGameClearSelection.h"
#include "Tutorial/StepGameConnectStructIntro.h"
#include "Tutorial/StepGameConquerCellsEnd.h"
#include "Tutorial/StepGameConquerCellsSimple.h"
#include "Tutorial/StepGameConquerEnergyGenIntro.h"
#include "Tutorial/StepGameConquerMaterialGenIntro.h"
#include "Tutorial/StepGameConquerStructSimple.h"
#include "Tutorial/StepGameDisableCamera.h"
#include "Tutorial/StepGameEndTurnSimple.h"
#include "Tutorial/StepGameEnemyIntro.h"
#include "Tutorial/StepGameIntro3.h"
#include "Tutorial/StepGameMoveCamera.h"
#include "Tutorial/StepGameMoveCameraOverObject.h"
#include "Tutorial/StepGameMoveUnitSimple.h"
#include "Tutorial/StepGameMoveUnitToCorner.h"
#include "Tutorial/StepGameOpenLootbox.h"
#include "Tutorial/StepGameQuickUnitButton.h"
#include "Tutorial/StepGameSelectObject.h"
#include "Tutorial/StepGameSetLootboxPrize.h"
#include "Tutorial/StepGameSetSelectionActiveAction.h"
#include "Tutorial/StepGameSetupResearch.h"
#include "Tutorial/StepGameSetupResearchIcon.h"
#include "Tutorial/StepGameSingleInfo.h"
#include "Tutorial/StepGameTechTreeDialog.h"
#include "Tutorial/StepGameTechTreeIcon.h"
#include "Tutorial/StepGameUnitConquerCellsIcon.h"
#include "Tutorial/StepGameUnit.h"
#include "Tutorial/StepGameUpgradeIntro.h"
#include "Tutorial/StepGameUpgradeUnit.h"
#include "Tutorial/StepGameUpgradeUnitFree.h"
#include "Tutorial/StepGameWaitLootboxOpen.h"
#include "Tutorial/StepGameWaitTurn.h"
#include "Tutorial/StepGameWallBuildIcon.h"
#include "Tutorial/StepGameWallBuildStart.h"
#include "Tutorial/StepGameWallBuildEnd.h"
#include "Tutorial/StepSaveGame.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/GameHUD.h"
#include "Widgets/PanelObjectActions.h"

namespace
{
using namespace game;

constexpr unsigned int indWorker1 = 0;
constexpr unsigned int indWorker2 = 1;
constexpr unsigned int indSoldier1 = 2;
constexpr unsigned int indSpawner1 = 3;

const Cell2D cellTLCity1(51, 51);
const Cell2D cellBRCity1(54, 54);
const Cell2D cellCityBlock2(54, 52);
const Cell2D cellCityBlock3(52, 52);
const Cell2D cellCityBlock4(52, 54);
const Cell2D cellLootbox1(74, 63);
const Cell2D cellRadar(41, 41);
const Cell2D cellResCenter(45, 47);
const Cell2D cellSpecialLootbox1(61, 73);
const Cell2D cellMatGen1(66, 62);
const Cell2D cellMatGen2(41, 49);
const Cell2D cellMatGen3(51, 72);
const Cell2D cellEneGen1(62, 66);
const Cell2D cellEneGen2(49, 41);
const Cell2D cellEneGen3(73, 53);
const Cell2D cellTower1(58, 58);
const Cell2D cellTower2(48, 48);
const Cell2D cellTower3(41, 58);
const Cell2D cellTower4(56, 42);
const Cell2D cellBunker1(37, 50);
const Cell2D cellBunker2(50, 37);
const Cell2D cellBarracks(53, 47);
const Cell2D cellEnemy1(63,52);

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
    AddStep([game, gs, panelActions]
        {
            return new StepGameBuildUnitStart(game, gs, panelActions,
                                              PanelObjectActions::BTN_BUILD_UNIT_BASE);
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
    AddStep([game, gs, panelActions]
        {
           const core::Pointd2D p0(1100, 300);
            return new StepGameBuildStructIntro(game, gs, panelActions, "TUT_GAME_BUILD_RES_GEN", p0);
        });
    AddStep([hud]
        {
            return new StepGameBuildStructure(hud, "TUT_GAME_CAT_RES_GEN", nullptr,
                                              TutorialConstants::catResources,
                                              TutorialConstants::structSolarPanel);
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
    AddStep([game, gs, panelActions]
        {
            const core::Pointd2D p0(1100, 250);
            return new StepGameBuildStructIntro(game, gs, panelActions, "TUT_GAME_BUILD_MORE", p0);
        });
    AddStep([hud]
        {
            const int indStruct = 0;
            return new StepGameBuildStructure(hud, "TUT_GAME_CAT_RES_GEN", nullptr,
                                              TutorialConstants::catResources,
                                              TutorialConstants::structSolarPanel);
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
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
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
    AddStep([game, gs,panelActions]
        {
            const core::Pointd2D p0(1100, 250);
            return new StepGameBuildStructIntro(game, gs, panelActions, "TUT_GAME_BUILD_MORE", p0);
        });
    AddStep([hud]
        {
            return new StepGameBuildStructure(hud, "TUT_GAME_CAT_RES_GEN", "TUT_GAME_BUILD_MAT_EXTR",
                                              TutorialConstants::catResources,
                                              TutorialConstants::structMatExtr);
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
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
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
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
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
            const int prizeQuantity = 350;
            return new StepGameSetLootboxPrize(lootbox, prizeType, prizeQuantity);
        });
    AddStep([this, local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker1);
            const GameObject * lootbox = GetObjectInCell(cellLootbox1);
            const core::Pointd2D p0(500, 100);

            return new StepGameOpenLootbox(game, unit, lootbox, isoMap, "TUT_GAME_LOOTBOX_1", p0);
        });
    AddStep([this]
        {
            const GameObject * lootbox = GetObjectInCell(cellLootbox1);
            return new StepGameWaitLootboxOpen(GetGameMap(), lootbox);
        });
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
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
    // CONQUER MATERIAL GENERATOR WITH WORKER 1
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
    AddStep([game, gs, panelActions]
        {
            return new StepGameBuildUnitStart(game, gs, panelActions,
                                              PanelObjectActions::BTN_BUILD_UNIT_BASE);
        });
    AddStep([hud] { return new StepGameBuildUnitEnd(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([localBase] { return new StepDelay(localBase->GetTimeBuildUnit()); });
    // CLEAR SELECTION
    AddStep([gs] { return new StepGameClearSelection(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 2
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(1300, 450);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    // MOVE CAMERA
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game]
            {
                const int movX = 0;
                const int movY = -350;
                return new StepGameMoveCamera(movX, movY);
            });
     // START TO CONNECT MATERIAL GENERATOR WITH WORKER 2
    AddStep([]
        {
            const core::Pointd2D p0(1000, 200);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1000, 200);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(74, 72);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(900, 100);
            const Cell2D cellEnd(74, 66);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONTINUE TO CONNECT MATERIAL GENERATOR WITH WORKER 2
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1000, 200);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(73, 66);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(900, 100);
            const Cell2D cellEnd(70, 62);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 1
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(1300, 450);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // FINISH TO CONNECT MATERIAL GENERATOR WITH WORKER 1
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1000, 200);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(67, 62);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(900, 100);
            const Cell2D cellEnd(69, 62);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER ENERGY GENERATOR WITH WORKER 1
    AddStep([this, local]
        {
            const GameObject * gen = GetObjectInCell(cellEneGen1);
            const core::Pointd2D p0(800, 150);
            return new StepGameConquerEnergyGenIntro(gen, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const GameObject * gen = GetObjectInCell(cellEneGen1);
            const core::Pointd2D p0(850, 200);
            return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
        });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, IDLE); });
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // SELECT WORKER 2
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(1300, 450);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT ENERGY GENERATOR WITH WORKER 2
    AddStep([]
        {
            const core::Pointd2D p0(1150, 150);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1000, 200);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(65, 63);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(1200, 150);
            const Cell2D cellEnd(63, 65);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    // SELECT WORKER 1
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(1200, 350);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // OPEN SPECIAL LOOTBOX
    AddStep([this]
        {
            auto lootbox = static_cast<LootBox *>(GetObjectInCell(cellSpecialLootbox1));
            const unsigned int prizeType = LootBox::LB_NULL;
            const int prizeQuantity = 0;
            return new StepGameSetLootboxPrize(lootbox, prizeType, prizeQuantity);
        });
    AddStep([this, local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker1);
            const GameObject * lootbox = GetObjectInCell(cellSpecialLootbox1);
            const core::Pointd2D p0(1000, 200);

            return new StepGameOpenLootbox(game, unit, lootbox, isoMap, "TUT_GAME_LOOTBOX_1b", p0);
        });
    AddStep([this]
        {
            const GameObject * lootbox = GetObjectInCell(cellSpecialLootbox1);
            return new StepGameWaitLootboxOpen(GetGameMap(), lootbox);
        });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([]
        {
            const core::Pointd2D p0(1150, 350);
            return new StepGameSingleInfo(p0, "TUT_GAME_NO_LUCK");
        });
    AddStep([] { return new StepDelay(0.5f); });
    // UPGRADE WORKER 1
    AddStep([game, gs, panelActions]
        {
            core::Pointd2D p0(900, 150);
            return new StepGameUpgradeIntro(game, gs, panelActions, "TUT_GAME_UPGRADE_1b", p0);
        });
    AddStep([hud] { return new StepGameUpgradeUnit(hud, false); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA
    AddStep([game]
            {
                const int movX = -200;
                const int movY = -200;
                return new StepGameMoveCamera(movX, movY);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD DEFENSIVE TOWER WITH WORKER 1
    AddStep([game, gs, panelActions]
            {
                const core::Pointd2D p0(1000, 250);
                return new StepGameBuildStructIntro(game, gs, panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
            });
    AddStep([hud]
            {
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_DTOWER_4",
                                                  TutorialConstants::catDefenses,
                                                  TutorialConstants::structDefTower);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(900, 250);
                return new StepGameBuildTowerEnd(isoMap, unit, cellTower1, p0);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // START TO CONNECT DEFENSIVE TOWER WITH WORKER 1
    AddStep([]
        {
            const core::Pointd2D p0(1000, 200);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1000, 200);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(58, 59);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(900, 100);
            const Cell2D cellEnd(58, 60);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    // SELECT WORKER 2
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(1100, 350);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // CONTINUE TO CONNECT DEFENSIVE TOWER WITH WORKER 2
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1000, 200);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(61, 64);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(900, 100);
            const Cell2D cellEnd(58, 61);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // INTRODUCE FIRST CITY AREA
    AddStep([isoMap]
        {
            const core::Pointd2D p0(1000, 300);

            return new StepGameCityIntro(cellTLCity1, cellBRCity1, isoMap, p0);
        });
    // SELECT WORKER 1
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(1100, 350);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // START TO CONQUER CITY WITH WORKER 1
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const GameObject * gen = GetObjectInCell(cellBRCity1);
            const core::Pointd2D p0(1200, 350);
            return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
        });
    // SELECT WORKER 2
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(1100, 350);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT CITY WITH WORKER 2
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1050, 350);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(57, 58);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(950, 250);
            const Cell2D cellEnd(54, 55);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    // UPGRADE WORKER 2
    AddStep([game, gs, panelActions]
        {
            core::Pointd2D p0(900, 150);
            return new StepGameUpgradeIntro(game, gs, panelActions, "TUT_GAME_UPGRADE_1b", p0);
        });
    AddStep([hud] { return new StepGameUpgradeUnit(hud, false); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // SELECT WORKER 1
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(1100, 350);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER ALL CITY WITH WORKER 1
    AddStep([]
    {
        const core::Pointd2D p0(600, 150);
        return new StepGameSingleInfo(p0, "TUT_GAME_CITY_2");
    });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const GameObject * gen = GetObjectInCell(cellCityBlock2);
            const core::Pointd2D p0(1100, 250);
            return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const GameObject * gen = GetObjectInCell(cellCityBlock4);
            const core::Pointd2D p0(500, 150);
            return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const GameObject * gen = GetObjectInCell(cellCityBlock3);
            const core::Pointd2D p0(1200, 200);
            return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
        });
        // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 2
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(1100, 350);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD DEFENSIVE TOWER WITH WORKER 2
    AddStep([game, gs, panelActions]
            {
                const core::Pointd2D p0(1000, 250);
                return new StepGameBuildStructIntro(game, gs, panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
            });
    AddStep([hud]
            {
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_DTOWER_4",
                                                  TutorialConstants::catDefenses,
                                                  TutorialConstants::structDefTower);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indWorker2);
                const core::Pointd2D p0(950, 250);
                return new StepGameBuildTowerEnd(isoMap, unit, cellTower2, p0);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // CONNECT DEFENSIVE TOWER WITH WORKER 2
    AddStep([]
        {
            const core::Pointd2D p0(1000, 200);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1000, 200);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            Cell2D target(49, 48);

            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(900, 100);
            Cell2D cellEnd(51, 50);

            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    // SELECT WORKER 1
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(1100, 350);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER MATERIAL GENERATOR WITH WORKER 1
    AddStep([this, local]
        {
            const GameObject * gen = GetObjectInCell(cellMatGen2);
            const core::Pointd2D p0(450, 200);
            return new StepGameConquerMaterialGenIntro(gen, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const GameObject * gen = GetObjectInCell(cellMatGen2);
            const core::Pointd2D p0(550, 150);
            return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // SELECT WORKER 2
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(1000, 500);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT MATERIAL GENERATOR WITH WORKER 2
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1050, 350);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(47, 48);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(950, 250);
            const Cell2D cellEnd(42, 48);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    // SELECT WORKER 1
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(1100, 350);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER ENERGY GENERATOR WITH WORKER 1
    AddStep([this, local]
        {
            const GameObject * gen = GetObjectInCell(cellEneGen2);
            const core::Pointd2D p0(1000, 150);
            return new StepGameConquerEnergyGenIntro(gen, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const GameObject * gen = GetObjectInCell(cellEneGen2);
            const core::Pointd2D p0(950, 200);
            return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT ENERGY GENERATOR WITH WORKER 1
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1050, 350);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(48, 42);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(950, 250);
            const Cell2D cellEnd(48, 47);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // UPGRADE WORKER 1
    AddStep([game, gs, panelActions]
            {
                core::Pointd2D p0(1000, 300);
                return new StepGameUpgradeIntro(game, gs, panelActions, "TUT_GAME_UPGRADE_1b", p0);
            });
    AddStep([hud] { return new StepGameUpgradeUnitFree(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    // TODO remove this save when finished tutorial
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 2
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(1000, 500);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD RESEARCH CENTER
    AddStep([game, gs, panelActions]
        {
            const core::Pointd2D p0(1100, 500);
            return new StepGameBuildStructIntro(game, gs, panelActions, "TUT_GAME_BUILD_RES_CEN_1", p0);
        });
    AddStep([hud]
        {
            return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_RES_CEN_2", nullptr,
                                              TutorialConstants::catTech,
                                              TutorialConstants::structResearchCenter);
        });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(400, 250);
            return new StepGameBuildTowerEnd(isoMap, unit, cellResCenter, p0);
        });
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // SETUP RESEARCH
    AddStep([this, game, isoMap]
        {
            const core::Pointd2D p0(1100, 600);
            const GameObject * obj = GetObjectInCell(cellResCenter);
            return new StepGameSelectObject(game, isoMap, obj, "TUT_GAME_RES_CEN_1", p0);
        });
    AddStep([game, gs, panelActions]
        {
            const core::Pointd2D p0(1000, 650);
            return new StepGameSetupResearchIcon(game, gs, panelActions, p0);
        });
    AddStep([hud] { return new StepGameSetupResearch(hud, { 100, 50, 50 }); });
    // SELECT WORKER 2
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(1100, 350);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD BUNKER WITH WORKER 2
    AddStep([game, gs, panelActions]
        {
            const core::Pointd2D p0(1000, 250);
            return new StepGameBuildStructIntro(game, gs, panelActions, "TUT_GAME_BUILD_BUNKER_1", p0);
        });
    AddStep([hud]
        {
            return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3", nullptr,
                                              TutorialConstants::catDefenses,
                                              TutorialConstants::structBunker);
        });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(900, 200);
            return new StepGameBuildTowerEnd(isoMap, unit, cellBunker1, p0);
        });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    // SELECT WORKER 1
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(600, 500);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    // MOVE CAMERA
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game]
        {
            const int movX = -400;
                const int movY = 0;
                return new StepGameMoveCamera(movX, movY);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD BUNKER WITH WORKER 1
    AddStep([game, gs, panelActions]
        {
            const core::Pointd2D p0(1000, 250);
            return new StepGameBuildStructIntro(game, gs, panelActions, "TUT_GAME_BUILD_BUNKER_1", p0);
        });
    AddStep([hud]
        {
            return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3", nullptr,
                                              TutorialConstants::catDefenses,
                                              TutorialConstants::structBunker);
        });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(900, 250);
            return new StepGameBuildTowerEnd(isoMap, unit, cellBunker2, p0);
        });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT BUNKER WITH WORKER 1
    AddStep([]
        {
            const core::Pointd2D p0(1100, 350);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1050, 350);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(50, 38);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(950, 250);
            const Cell2D cellEnd(50, 40);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL WITH WORKER 1
    AddStep([] { return new StepGameWallBuildIntro(); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game, gs, panelActions] { return new StepGameWallBuildIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
            const Cell2D target(50, 38);
            return new StepGameWallBuildStart(game, isoMap, cellStart, target);
        });
    AddStep([isoMap, local, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const Cell2D cellEnd(52, 38);
            const core::Pointd2D p0(1100, 250);
            return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
        });
    // SELECT WORKER 2
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([hud]
        {
            const sgl::core::Pointd2D p0(100, 600);
            return new StepGameQuickUnitButton(hud, indWorker2, nullptr, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT BUNKER WITH WORKER 2
    AddStep([]
        {
            const core::Pointd2D p0(1100, 350);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1050, 350);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(38, 50);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(1100, 550);
            const Cell2D cellEnd(40, 50);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL WITH WORKER 2
    AddStep([] { return new StepGameWallBuildIntro(); });
    AddStep([game, gs, panelActions] { return new StepGameWallBuildIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
                const Cell2D target(38, 50);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([isoMap, local, game]
            {
                const auto unit = local->GetUnit(indWorker2);
                const Cell2D cellEnd(38, 52);
                const core::Pointd2D p0(1100, 250);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // INTRODUCE MOVE TO MATERIAL GENERATOR
    AddStep([]
        {
            const core::Pointd2D p0(600, 250);
            return new StepGameSingleInfo(p0, "TUT_GAME_CONT_EXPL");
        });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA
    AddStep([game]
        {
            const int movX = 0;
            const int movY = 350;
            return new StepGameMoveCamera(movX, movY);
        });
    // MOVE WORKER 2 TO MATERIAL GENERATOR
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker2);
            const Cell2D target(51, 66);
            const core::Pointd2D p0(450, 200);
            return new StepGameMoveUnitSimple(game, unit, isoMap, target, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // UPGRADE WORKER 2
    AddStep([game, gs, panelActions]
            {
                core::Pointd2D p0(1000, 300);
                return new StepGameUpgradeIntro(game, gs, panelActions, "TUT_GAME_UPGRADE_1b", p0);
            });
    AddStep([hud] { return new StepGameUpgradeUnitFree(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 1
    AddStep([hud]
        {
            const sgl::core::Pointd2D p0(200, 600);
            return new StepGameQuickUnitButton(hud, indWorker1, nullptr, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // INTRODUCE MOVE TO ENERGY GENERATOR
    AddStep([]
        {
            const core::Pointd2D p0(600, 250);
            return new StepGameSingleInfo(p0, "TUT_GAME_CONT_EXPL");
        });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA
    AddStep([game]
        {
            const int movX = 0;
            const int movY = 500;
            return new StepGameMoveCamera(movX, movY);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE WORKER 1 TO MATERIAL GENERATOR
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const Cell2D target(68, 53);
            const core::Pointd2D p0(450, 200);
            return new StepGameMoveUnitSimple(game, unit, isoMap, target, p0);
        });
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER ENERGY GENERATOR WITH WORKER 1
    AddStep([this, local]
        {
            const GameObject * gen = GetObjectInCell(cellEneGen3);
            const core::Pointd2D p0(800, 150);
            return new StepGameConquerEnergyGenIntro(gen, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const GameObject * gen = GetObjectInCell(cellEneGen3);
            const core::Pointd2D p0(850, 200);
            return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT ENERGY GENERATOR WITH WORKER 1
    AddStep([]
            {
                const core::Pointd2D p0(1100, 350);
                return new StepGameConnectStructIntro(p0);
            });
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(1100, 400);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(71, 53);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(1100, 300);
                const Cell2D cellEnd(71, 61);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 2
    AddStep([hud]
        {
            const sgl::core::Pointd2D p0(200, 600);
            return new StepGameQuickUnitButton(hud, indWorker2, nullptr, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER MATERIAL GENERATOR WITH WORKER 2
    AddStep([this, local]
        {
            const GameObject * gen = GetObjectInCell(cellMatGen3);
            const core::Pointd2D p0(1250, 350);
            return new StepGameConquerMaterialGenIntro(gen, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker2);
            const GameObject * gen = GetObjectInCell(cellMatGen3);
            const core::Pointd2D p0(400, 200);
            return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // START TO CONNECT MATERIAL GENERATOR WITH WORKER 2
    AddStep([]
            {
                const core::Pointd2D p0(1100, 350);
                return new StepGameConnectStructIntro(p0);
            });
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(1100, 400);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(51, 70);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker2);
                const core::Pointd2D p0(1100, 300);
                const Cell2D cellEnd(54, 66);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONTINUE TO CONNECT MATERIAL GENERATOR WITH WORKER 2
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(1100, 400);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(54, 65);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker2);
                const core::Pointd2D p0(1100, 300);
                const Cell2D cellEnd(58, 64);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    // SELECT WORKER 1
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(1300, 450);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE WORKER 1 BACK TO FRONT
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const Cell2D target(55, 51);
            const core::Pointd2D p0(450, 200);
            return new StepGameMoveUnitSimple(game, unit, isoMap, target, p0);
        });
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT RESEARCH CENTER AND UNLOCK UPGRADE
    AddStep([this]
            {
                const GameObject * obj = GetObjectInCell(cellResCenter);
                const float speed = 800.f;
                return new StepGameMoveCameraOverObject(obj, speed);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([]
            {
                const core::Pointd2D p0(685, 200);
                return new StepGameSingleInfo(p0, "TUT_GAME_RES_CEN_2");
            });
    AddStep([this, game, isoMap]
            {
                const core::Pointd2D p0(710, 250);
                const GameObject * obj = GetObjectInCell(cellResCenter);
                return new StepGameSelectObject(game, isoMap, obj, "TUT_GAME_RES_CEN_1", p0);
            });
    AddStep([game, gs, panelActions]
            {
                const core::Pointd2D p0(1000, 650);
                return new StepGameTechTreeIcon(game, gs, panelActions, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([hud] { return new StepGameTechTreeDialog(hud, TECH_UP_RADAR_STATION, false); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    // CLEAR SELECTION
    AddStep([gs] { return new StepGameClearSelection(gs); });
    // SELECT WORKER 1
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(1200, 450);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD RADAR STATION
    AddStep([game, gs, panelActions]
        {
            const core::Pointd2D p0(1100, 500);
            return new StepGameBuildStructIntro(game, gs, panelActions, "TUT_GAME_BUILD_RADAR_1", p0);
        });
    AddStep([hud]
        {
            return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_RES_CEN_2",
                                              "TUT_GAME_BUILD_RADAR_2", TutorialConstants::catTech,
                                              TutorialConstants::structRadarStation);
        });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(1100, 250);
            return new StepGameBuildTowerEnd(isoMap, unit, cellRadar, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 2
    AddStep([hud]
        {
            const sgl::core::Pointd2D p0(200, 600);
            return new StepGameQuickUnitButton(hud, indWorker2, nullptr, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA
    AddStep([game]
        {
            const int movX = 250;
            const int movY = -250;
            return new StepGameMoveCamera(movX, movY);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD BUNKER WITH WORKER 2
    AddStep([game, gs, panelActions]
            {
                const core::Pointd2D p0(1000, 250);
                return new StepGameBuildStructIntro(game, gs, panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
            });
    AddStep([hud]
            {
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_DTOWER_4",
                                                  TutorialConstants::catDefenses,
                                                  TutorialConstants::structDefTower);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indWorker2);
                const core::Pointd2D p0(950, 300);
                return new StepGameBuildTowerEnd(isoMap, unit, cellTower3, p0);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL WITH WORKER 2
    AddStep([] { return new StepGameWallBuildIntro(); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game, gs, panelActions] { return new StepGameWallBuildIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
                const Cell2D target(42, 58);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([isoMap, local, game]
            {
                const auto unit = local->GetUnit(indWorker2);
                const Cell2D cellEnd(43, 58);
                const core::Pointd2D p0(1100, 250);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 1
    AddStep([hud]
        {
            const sgl::core::Pointd2D p0(200, 600);
            return new StepGameQuickUnitButton(hud, indWorker1, nullptr, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT RADAR STATION WITH WORKER 1
    AddStep([]
        {
            const core::Pointd2D p0(1000, 250);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1050, 350);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(41, 42);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(950, 250);
            const Cell2D cellEnd(41, 47);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT WALL WITH WORKER 1
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1050, 350);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(43, 49);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(950, 250);
            const Cell2D cellEnd(43, 57);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    // SELECT WORKER 2
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(1300, 450);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD GATE WITH WORKER 2
    AddStep([game, gs, panelActions]
        {
            const core::Pointd2D p0(1100, 400);
            return new StepGameBuildStructIntro(game, gs, panelActions, "TUT_GAME_BUILD_GATE_1", p0);
        });
    AddStep([hud]
        {
            return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                              "TUT_GAME_BUILD_GATE_2",
                                              TutorialConstants::catDefenses,
                                              TutorialConstants::structGate);
        });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(1100, 450);
            const Cell2D target(44, 58);
            return new StepGameBuildTowerEnd(isoMap, unit, target, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL WITH WORKER 2
    AddStep([game, gs, panelActions] { return new StepGameWallBuildIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
                const Cell2D target(45, 58);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([isoMap, local, game]
            {
                const auto unit = local->GetUnit(indWorker2);
                const Cell2D cellEnd(46, 58);
                const core::Pointd2D p0(1100, 250);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA
    AddStep([game]
        {
            const int movX = -150;
            const int movY = -150;
            return new StepGameMoveCamera(movX, movY);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE UNIT 2
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker2);
            const Cell2D target(49, 45);
            const core::Pointd2D p0(450, 200);
            return new StepGameMoveUnitSimple(game, unit, isoMap, target, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 1
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(1300, 450);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD DEFENSIVE TOWER WITH WORKER 1
    AddStep([game, gs, panelActions]
            {
                const core::Pointd2D p0(1000, 250);
                return new StepGameBuildStructIntro(game, gs, panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
            });
    AddStep([hud]
            {
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_DTOWER_3",
                                                  TutorialConstants::catDefenses,
                                                  TutorialConstants::structDefTower);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(950, 250);
                return new StepGameBuildTowerEnd(isoMap, unit, cellTower4, p0);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL WITH WORKER 1
    AddStep([] { return new StepGameWallBuildIntro(); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game, gs, panelActions] { return new StepGameWallBuildIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
                const Cell2D target(57, 42);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([isoMap, local, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const Cell2D cellEnd(57, 43);
                const core::Pointd2D p0(1100, 250);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    // BUILD GATE WITH WORKER 1
    AddStep([game, gs, panelActions]
        {
            const core::Pointd2D p0(1100, 400);
            return new StepGameBuildStructIntro(game, gs, panelActions, "TUT_GAME_BUILD_GATE_1", p0);
        });
    AddStep([hud]
        {
            return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                              "TUT_GAME_BUILD_GATE_2",
                                              TutorialConstants::catDefenses,
                                              TutorialConstants::structGate);
        });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(1100, 450);
            const Cell2D target(57, 44);
            return new StepGameBuildTowerEnd(isoMap, unit, target, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL WITH WORKER 1
    AddStep([game, gs, panelActions] { return new StepGameWallBuildIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
                const Cell2D target(57, 45);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([isoMap, local, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const Cell2D cellEnd(57, 46);
                const core::Pointd2D p0(1100, 250);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 2
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(1300, 450);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER CELLS TO CONNECT WALL WITH WORKER 2
    AddStep([]
            {
                const core::Pointd2D p0(1100, 350);
                return new StepGameConnectStructIntro(p0);
            });
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(1100, 400);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(49, 45);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker2);
                const core::Pointd2D p0(400, 300);
                const Cell2D cellEnd(56, 45);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD BARRACKS WITH WORKER 2
    AddStep([game, gs, panelActions]
            {
                const core::Pointd2D p0(1000, 625);
                return new StepGameBuildStructIntro(game, gs, panelActions, "TUT_GAME_BUILD_BARRACKS_1", p0);
            });
    AddStep([hud] { return new StepGameBuildUnitEnd(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(900, 650);
            return new StepGameBuildTowerEnd(isoMap, unit, cellBarracks, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD SOLDIER
    AddStep([this, game, isoMap]
        {
            const core::Pointd2D p0(500, 200);
            const GameObject * barracks = GetObjectInCell(cellBarracks);
            return new StepGameSelectObject(game, isoMap, barracks, "TUT_GAME_BARRACKS_1", p0);
        });
    AddStep([game, gs, panelActions]
        {
            return new StepGameBuildUnitStart(game, gs, panelActions,
                                              PanelObjectActions::BTN_BUILD_UNIT_BARRACKS);
        });
    AddStep([hud] { return new StepGameBuildUnitEnd(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([this]
        {
            const auto barracks = static_cast<Structure *>(GetObjectInCell(cellBarracks));
            return new StepDelay(barracks->GetTimeBuildUnit());
        });
    // CLEAR SELECTION
    AddStep([gs] { return new StepGameClearSelection(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE VIEW BACK TO BASE
    AddStep([panelTurn, game]
        {
            const core::Pointd2D p0(50, 600);
            return new StepGameBackToBase(panelTurn, "TUT_GAME_BACK_TO_BASE_1", p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD SPAWNER
    AddStep([]
        {
            const core::Pointd2D p0(300, 300);
            return new StepGameSingleInfo(p0, "TUT_GAME_BUILD_SPAWNER_1");
        });
    AddStep([localBase, game, isoMap]
        {
            const core::Pointd2D p0(350, 300);
            return new StepGameSelectObject(game, isoMap, localBase, "TUT_GAME_BASE_4", p0);
        });
    AddStep([game, gs, panelActions]
        {
            return new StepGameBuildUnitStart(game, gs, panelActions,
                                              PanelObjectActions::BTN_BUILD_UNIT_BASE);
        });
    AddStep([hud]
        {
            return new StepGameBuildStructure(hud, nullptr, "TUT_GAME_BUILD_SPAWNER_2",
                                              TutorialConstants::catUnitGeneric,
                                              TutorialConstants::unitMiniSpawner);
        });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([localBase] { return new StepDelay(localBase->GetTimeBuildUnit()); });
    // CLEAR SELECTION
    AddStep([gs] { return new StepGameClearSelection(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT SPAWNER
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indSpawner1);
            const core::Pointd2D p0(1300, 450);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // ADD ENEMY NEAR BASE
    AddStep([]
            {
                const core::Pointd2D p0(600, 150);
                return new StepGameEnemyIntro(p0);
            });
    AddStep([this, playerAI]
            {
                return new StepGameAddEnemy(GetGameMap(), playerAI, ObjectData::TYPE_UNIT_SOLDIER1,
                                            cellEnemy1, true);
            });
    // MOVE CAMERA
    AddStep([game]
            {
                const int movX = -250;
                const int movY = -250;
                return new StepGameMoveCamera(movX, movY);
            });
    // MOVE SPAWNER NEAR ENEMY
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indSpawner1);
            const Cell2D target(70, 63);
            const core::Pointd2D p0(1100, 300);
            return new StepGameMoveUnitSimple(game, unit, isoMap, target, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
}

TutorialGame3::~TutorialGame3()
{

}

} // namespace game
