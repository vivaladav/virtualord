#include "Tutorial/TutorialGame1.h"

#include "CameraMapController.h"
#include "Game.h"
#include "GameMap.h"
#include "Player.h"
#include "AI/PlayerAI.h"
#include "GameObjects/Base.h"
#include "GameObjects/ObjectData.h"
#include "GameObjects/Unit.h"
#include "Indicators/OverlayCellConquest.h"
#include "Indicators/OverlayWall.h"
#include "Screens/ScreenGame.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepGameAddEnemy.h"
#include "Tutorial/StepGameBackToBase.h"
#include "Tutorial/StepGameBase.h"
#include "Tutorial/StepGameBaseBuildUnit.h"
#include "Tutorial/StepGameBaseBuildUnitIcon.h"
#include "Tutorial/StepGameBuildStructIntro.h"
#include "Tutorial/StepGameBuildStructure.h"
#include "Tutorial/StepGameBuildTowerEnd.h"
#include "Tutorial/StepGameBuildUnitEnd.h"
#include "Tutorial/StepGameBuildUnitStart.h"
#include "Tutorial/StepGameClearSelection.h"
#include "Tutorial/StepGameConnectStructIntro.h"
#include "Tutorial/StepGameConquerCells.h"
#include "Tutorial/StepGameConquerCellsContinue.h"
#include "Tutorial/StepGameConquerCellsEnd.h"
#include "Tutorial/StepGameConquerCellsSimple.h"
#include "Tutorial/StepGameConquerEnergyGenIntro.h"
#include "Tutorial/StepGameConquerMaterialGenIntro.h"
#include "Tutorial/StepGameConquerStruct.h"
#include "Tutorial/StepGameConquerStructSimple.h"
#include "Tutorial/StepGameDisableCamera.h"
#include "Tutorial/StepGameEnableCamera.h"
#include "Tutorial/StepGameEndTurn.h"
#include "Tutorial/StepGameEndTurnIntro.h"
#include "Tutorial/StepGameEndTurnSimple.h"
#include "Tutorial/StepGameEnemyIntro.h"
#include "Tutorial/StepGameEnemyKilled.h"
#include "Tutorial/StepGameEnergyRegeneration.h"
#include "Tutorial/StepGameIntro.h"
#include "Tutorial/StepGameMapNavigation.h"
#include "Tutorial/StepGameMaterialGenerator.h"
#include "Tutorial/StepGameMissionGoalsIcon.h"
#include "Tutorial/StepGameMissionGoalsDialog.h"
#include "Tutorial/StepGameMoveCamera.h"
#include "Tutorial/StepGameMoveCameraOverCell.h"
#include "Tutorial/StepGameMoveCameraOverObject.h"
#include "Tutorial/StepGameMoveUnit.h"
#include "Tutorial/StepGameMoveUnitSimple.h"
#include "Tutorial/StepGameMoveUnitToCorner.h"
#include "Tutorial/StepGameObjectAttackSimple.h"
#include "Tutorial/StepGamePrimaryMissionGoal.h"
#include "Tutorial/StepGameQuickUnitButton.h"
#include "Tutorial/StepGameSelectObject.h"
#include "Tutorial/StepGameSetObjectFatalHit.h"
#include "Tutorial/StepGameSetObjectHealth.h"
#include "Tutorial/StepGameSetObjectPerfectShot.h"
#include "Tutorial/StepGameSetSelectionAllowed.h"
#include "Tutorial/StepGameSingleInfo.h"
#include "Tutorial/StepGameStructConnected.h"
#include "Tutorial/StepGameStructDisconnected.h"
#include "Tutorial/StepGameTestCameraFocus.h"
#include "Tutorial/StepGameTowerIntro.h"
#include "Tutorial/StepGameTurnEnergy.h"
#include "Tutorial/StepGameUnit.h"
#include "Tutorial/StepGameUnitAttackIcon.h"
#include "Tutorial/StepGameUnitConquerCellsIcon.h"
#include "Tutorial/StepGameUpgradeIntro.h"
#include "Tutorial/StepGameUpgradeUnit.h"
#include "Tutorial/StepGameWaitEnemyKilled.h"
#include "Tutorial/StepGameWaitObjectIdle.h"
#include "Tutorial/StepGameWaitTurn.h"
#include "Tutorial/StepGameWallBuildEnd.h"
#include "Tutorial/StepGameWallBuildIcon.h"
#include "Tutorial/StepGameWallBuildIntro.h"
#include "Tutorial/StepGameWallBuildStart.h"
#include "Tutorial/StepSaveGame.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/GameHUD.h"
#include "Widgets/PanelObjectActions.h"

#ifdef DEBUG
#include <cassert>
#endif

namespace
{
using namespace game;

const Cell2D cellDT1(27, 16);
const Cell2D cellDT2(47, 24);
const Cell2D cellDT3(48, 33);
const Cell2D cellEnemy1(27, 18);
const Cell2D cellEnemy2(52, 37);
const Cell2D cellMatGen1(32, 10);
const Cell2D cellMatGen2(52, 20);
const Cell2D cellEneGen2(41, 26);

constexpr unsigned int indUnit1 = 0;
// TODO change this back to 1 when testing full tutorial
constexpr unsigned int indUnit2 = 1;

}

namespace game
{

TutorialGame1::TutorialGame1(Screen * screen)
    : TutorialGame(screen, TUTORIAL_MISSION_1)
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
    // pause before start
    AddStep([] { return new StepDelay(0.5f); });

    // ===== PART 1 =====
    // INTRO
    AddStep([] { return new StepGameIntro; });
    AddStep([] { return new StepDelay(0.5f); });
    // BASE INTRO
    AddStep([game, isoMap, localBase] { return new StepGameBase(game, isoMap, localBase); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD NEW UNIT
    AddStep([game, gs, panelActions] { return new StepGameBaseBuildUnitIcon(game, gs, panelActions); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([hud] { return new StepGameBaseBuildUnit(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([localBase] { return new StepDelay(localBase->GetTimeBuildUnit()); });
    // MOVE UNIT NEXT TO ENERGY GENERATOR
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indUnit1);
                const core::Pointd2D p0(1300, 450);
                return new StepGameUnit(game, isoMap, unit, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game, isoMap, local] { return new StepGameMoveUnit(game, local, isoMap); });
    // CONQUER ENERGY GENERATOR
    AddStep([this, local, isoMap, game]
            {
                const Cell2D cellGen(41, 11);
                const GameObject * gen = GetObjectInCell(cellGen);

                return new StepGameConquerStruct(game, local, gen, isoMap);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // EXPLAIN ENERGY AND END TURN
    AddStep([] { return new StepGameTurnEnergy(); });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurn(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // EXPLAIN ENERGY REGENERATION
    AddStep([] { return new StepGameEnergyRegeneration; });
    // EXPLAIN STRUCTURE CONNECTIONS AND CONNECT GENERATOR TO BASE
    AddStep([] { return new StepGameStructDisconnected; });
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            return new StepGameConquerCells(game, isoMap, cellStart);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit1);
            const Cell2D cellEnd(48, 10);
            const core::Pointd2D p0(1150, 500);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([]
        {
            const core::Pointd2D p0(1150, 450);
            return new StepGameSingleInfo(p0, "TUT_GAME_STRUCT_CONNECTED_1");
        });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER MATERIAL GENERATOR INTRO
    AddStep([game]
        {
            const int movX = 200;
            const int movY = -100;
            return new StepGameMoveCamera(movX, movY);
        });
    AddStep([this, local, game]
        {
            const GameObject * gen = GetObjectInCell(cellMatGen1);
            const core::Pointd2D p0(1100, 550);
            return new StepGameMaterialGenerator(gen, p0);
        });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // CONQUER MATERIAL GENERATOR
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit1);
            const GameObject * gen = GetObjectInCell(cellMatGen1);
            const core::Pointd2D p0(1300, 400);
            return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
        });
    // END TURN
    AddStep([] { return new StepGameEndTurnIntro(); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // CONNECT MATERIAL GENERATOR
    AddStep([]
        {
            const core::Pointd2D p0(1250, 200);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1200, 400);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(33, 10);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit1);
            const Cell2D cellEnd(39, 10);
            const core::Pointd2D p0(1150, 500);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // MOVE UNIT TO VISIBLE CORNER
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indUnit1);
                const Cell2D target(27, 15);
                const core::Pointd2D p0(1000, 100);
                return new StepGameMoveUnitToCorner(unit, isoMap, target, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // UPGRADE UNIT
    AddStep([game, gs, panelActions]
        {
            core::Pointd2D p0(900, 250);
            return new StepGameUpgradeIntro(game, gs, panelActions, "TUT_GAME_UPGRADE_1", p0);
        });
    AddStep([hud] { return new StepGameUpgradeUnit(hud, true); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // BUILD DEFENSIVE TOWER 1
    AddStep([game, gs, panelActions]
        {
            const core::Pointd2D p0(900, 250);

            return new StepGameBuildStructIntro(game, gs, panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
        });
    AddStep([hud]
            {
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_DTOWER_4",
                                                  "TUT_GAME_BASE_BUILD_UNIT_3",
                                                  TutorialConstants::catDefenses,
                                                  TutorialConstants::structDefTower);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
        {
            const auto unit = local->GetUnit(indUnit1);
            const core::Pointd2D p0(900, 250);
            return new StepGameBuildTowerEnd(isoMap, unit, cellDT1, p0);
        });
    // ENABLE PERFECT SHOT AND DISABLE FATAL HIT
    AddStep([this]
    {
        GameObject * tower = GetObjectInCell(cellDT1);
        return new StepGameSetObjectPerfectShot(tower, true, false);
    });
    AddStep([this]
    {
        GameObject * tower = GetObjectInCell(cellDT1);
        return new StepGameSetObjectFatalHit(tower, false);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT DEFENSIVE TOWER
    AddStep([game]
        {
            const core::Pointd2D p0(1000, 150);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(27, 15);
            const core::Pointd2D p0(1000, 250);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, isoMap, game]
        {
            const Cell2D target(27, 10);
            const core::Pointd2D p0(1000, 250);
            const auto overlay = GetOverlayCellConquest();

            return new StepGameConquerCellsContinue(game, isoMap, target, overlay, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit1);
            const Cell2D cellEnd(30, 10);
            const core::Pointd2D p0(1000, 300);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([] { return new StepGameStructConnected; });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // BUILD WALL FROM TOWER
    AddStep([] { return new StepGameWallBuildIntro(); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game, gs, panelActions] { return new StepGameWallBuildIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
                const Cell2D target(27, 15);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([isoMap, local, game]
            {
                const auto unit = local->GetUnit(indUnit1);
                const Cell2D cellEnd(24, 15);
                const core::Pointd2D p0(900, 150);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE VIEW BACK TO BASE
    AddStep([panelTurn, game]
    {
        const core::Pointd2D p0(1100, 200);
        return new StepGameBackToBase(panelTurn, "TUT_GAME_BACK_TO_BASE_1", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // ===== PART 2 =====
    // BUILD SECOND UNIT
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
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indUnit2);
                const core::Pointd2D p0(1300, 450);
                return new StepGameUnit(game, isoMap, unit, p0);
            });
    // CONQUER SECOND MATERIAL GENERATOR
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local]
        {
            const GameObject * gen = GetObjectInCell(cellMatGen2);

            return new StepGameConquerMaterialGenIntro(gen);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit2);
            const GameObject * gen = GetObjectInCell(cellMatGen2);
            const core::Pointd2D p0(1250, 550);
            return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
        });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // CONNECT SECOND MATERIAL GENERATOR
    AddStep([]
            {
                const core::Pointd2D p0(1150, 500);
                return new StepGameConnectStructIntro(p0);
            });
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(200, 550);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(51, 18);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit2);
            const core::Pointd2D p0(700, 250);
            const Cell2D cellEnd(51, 11);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // MOVE UNIT TO VISIBLE CORNER
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indUnit2);
                const Cell2D target(47, 23);
                const core::Pointd2D p0(1050, 300);
                return new StepGameMoveUnitToCorner(unit, isoMap, target, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD BUNKER
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(900, 250);
        return new StepGameBuildStructIntro(game, gs, panelActions, "TUT_GAME_BUILD_BUNKER_1", p0);
    });
    AddStep([hud]
    {
        return new StepGameBuildStructure(hud,"TUT_GAME_BUILD_DTOWER_3", nullptr,
                                          "TUT_GAME_BASE_BUILD_UNIT_3",
                                          TutorialConstants::catDefenses,
                                          TutorialConstants::structBunker);
    });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
        {
            const auto unit = local->GetUnit(indUnit2);
            const core::Pointd2D p0(900, 250);
            return new StepGameBuildTowerEnd(isoMap, unit, cellDT2, p0);
        });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // CONNECT DEFENSIVE TOWERs
    AddStep([]
        {
            const core::Pointd2D p0(1100, 450);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1100, 450);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(47, 23);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit2);
            const Cell2D cellEnd(50, 20);
            const core::Pointd2D p0(1100, 450);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // ===== PART 3 =====
    // ADD ENEMY NEAR TOWER 1
    AddStep([]
            {
                const core::Pointd2D p0(1100, 350);
                return new StepGameEnemyIntro(p0);
            });
    AddStep([this, playerAI]
            {
                return new StepGameAddEnemy(GetGameMap(), playerAI,
                                            ObjectData::TYPE_UNIT_WORKER1, cellEnemy1, true);
            });
    AddStep([this]
            {
                const float health = 35.f;
                GameObject * enemy = GetObjectInCell(cellEnemy1);
                return new StepGameSetObjectHealth(enemy, health);
            });
    AddStep([gs] { return new StepGameClearSelection(gs); });
    // EXPLAIN CAMERA MOVE AND MOVE TO TOWER 1
    AddStep([] { return new StepGameMapNavigation; });
    AddStep([gs] { return new StepGameSetSelectionAllowed(gs, false); });
    AddStep([this] { return new StepGameEnableCamera(GetCameraMapController()); });
    AddStep([this, local]
            {
                const GameObject * tower = GetObjectInCell(cellDT1);
                const int areaHalfW = 300;
                const int areaHalfH = 200;
                const int tlX = tower->GetX() - areaHalfW;
                const int tlY = tower->GetY() - areaHalfH;
                const int brX = tower->GetX() + areaHalfW;
                const int brY = tower->GetY() + areaHalfH;

                const auto cam = GetCameraMapController()->GetCamera();

                return new StepGameTestCameraFocus("TUT_GAME_MAP_NAVIGATION_5", cam,
                                                   tlX, tlY, brX, brY);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([gs] { return new StepGameSetSelectionAllowed(gs, true); });
    AddStep([this]
            {
                const GameObject * tower = GetObjectInCell(cellDT1);
                return new StepGameMoveCameraOverObject(tower);
            });
    // ENEMY KILL AFTER END TURN
    AddStep([] { return new StepGameTowerIntro; });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([this]
            {
                GameObject * enemy = GetObjectInCell(cellEnemy1);
                return new StepGameWaitEnemyKilled(enemy, GetGameMap());
            });
    // COMMENT ON ENEMY KILL
    AddStep([this] { return new StepGameEnemyKilled(); });
    AddStep([hud]
        {
            const sgl::core::Pointd2D p0(450, 620);
            return new StepGameQuickUnitButton(hud, indUnit2, "TUT_GAME_QUICK_SEL_1", p0);
        });
    // ===== PART 4 =====
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL FROM TOWER
    AddStep([] { return new StepGameWallBuildIntro(); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game, gs, panelActions] { return new StepGameWallBuildIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
                const Cell2D target(47, 25);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indUnit2);
                const Cell2D cellEnd(47, 28);
                const core::Pointd2D p0(900, 250);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // UPGRADE UNIT
    AddStep([game, gs, panelActions]
        {
            core::Pointd2D p0(1000, 550);
            return new StepGameUpgradeIntro(game, gs, panelActions, "TUT_GAME_UPGRADE_1b", p0);
        });
    AddStep([hud] { return new StepGameUpgradeUnit(hud, false); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // CONQUER SECOND ENERGY GENERATOR
    AddStep([this, local, game]
        {
            const GameObject * gen = GetObjectInCell(cellEneGen2);
            const core::Pointd2D p0(400, 200);
            return new StepGameConquerEnergyGenIntro(gen, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit2);
            const GameObject * gen = GetObjectInCell(cellEneGen2);
            const core::Pointd2D p0(400, 200);
            return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
        });
    // CONNECT SECOND ENERGY GENERATOR
    AddStep([]
            {
                const core::Pointd2D p0(1250, 350);
                return new StepGameConnectStructIntro(p0);
            });
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(1100, 350);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(42, 26);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indUnit2);
                const core::Pointd2D p0(1000, 550);
                const Cell2D cellEnd(46, 26);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // MOVE VIEW BACK TO BASE
    AddStep([panelTurn, game]
        {
            const core::Pointd2D p0(800, 650);
            return new StepGameBackToBase(panelTurn, "TUT_GAME_BACK_TO_BASE_1b", p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // ===== PART 5 =====
    // INTRODUCE MISSION GOALS
    AddStep([localBase, game, isoMap]
            {
                const core::Pointd2D p0(500, 200);
                return new StepGameSelectObject(game, isoMap, localBase, "TUT_GAME_BASE_4", p0);
            });
    AddStep([game, gs, panelActions] { return new StepGameMissionGoalsIcon(game, gs, panelActions, true); });
    AddStep([hud] { return new StepGameMissionGoalsDialog(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // BACK TO UNIT 2
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(450, 620);
                return new StepGameQuickUnitButton(hud, indUnit2, "TUT_GAME_QUICK_SEL_1", p0);
            });
    // BUILD DEFENSIVE TOWER 3 WITH WORKER 2
    AddStep([game, gs, panelActions]
        {
            const core::Pointd2D p0(900, 250);

            return new StepGameBuildStructIntro(game, gs, panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
        });
    AddStep([hud]
        {
            return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                              "TUT_GAME_BUILD_DTOWER_4",
                                              "TUT_GAME_BASE_BUILD_UNIT_3",
                                              TutorialConstants::catDefenses,
                                              TutorialConstants::structDefTower);
           });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
        {
            const auto unit = local->GetUnit(indUnit2);
            const core::Pointd2D p0(900, 250);
            return new StepGameBuildTowerEnd(isoMap, unit, cellDT3, p0);
        });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // ADD HIDDEN ENEMY
    AddStep([this, playerAI]
    {
        return new StepGameAddEnemy(GetGameMap(), playerAI, ObjectData::TYPE_UNIT_SCOUT1,
                                    cellEnemy2, true);
    });
    // SET ENEMY'S HEALTH 1-SHOT LOW
    AddStep([this]
    {
        const float health = 10.f;
        GameObject * enemy = GetObjectInCell(cellEnemy2);
        return new StepGameSetObjectHealth(enemy, health);
    });
    // CONNECT DEFENSIVE TOWER WITH WORKER 2
    AddStep([]
        {
            const core::Pointd2D p0(1100, 450);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([game, gs, panelActions] { return new StepGameUnitConquerCellsIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1100, 450);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(48, 32);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit2);
            const Cell2D cellEnd(48, 28);
            const core::Pointd2D p0(1100, 450);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA NEXT DEFENSIVE TOWER
    AddStep([isoMap]
    {
        const float speedCamera = 600.f;
        const Cell2D cell(cellDT3.row, cellDT3.col - 2);
        return new StepGameMoveCameraOverCell(cell, isoMap, speedCamera);
    });
    // SET PERFECT SHOT AND FATAL TO DEFENSIVE TOWER 3
    AddStep([this]
    {
        GameObject * tower = GetObjectInCell(cellDT3);
        return new StepGameSetObjectPerfectShot(tower, true, true);
    });
    // INTRODUCE ENEMY 2
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([]
    {
        const core::Pointd2D p0(1200, 550);
        return new StepGameEnemyIntro(p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT DEFENSIVE TOWER
    AddStep([]
    {
        const core::Pointd2D p0(1200, 300);
        return new StepGameSingleInfo(p0, "TUT_GAME_DEF_TOWER_1");
    });
    AddStep([this, game, isoMap]
    {
        const core::Pointd2D p0(1200, 400);
        const GameObject * tower = GetObjectInCell(cellDT3);
        return new StepGameSelectObject(game, isoMap, tower, "TUT_GAME_DEF_TOWER_2", p0);
    });
    // SET ACTION ATTACK
    AddStep([this, game, gs, panelActions]
    {
        const core::Pointd2D p0(150, 700);
        return new StepGameUnitAttackIcon(game, gs, panelActions, "TUT_GAME_ATTACK_2b", p0);
    });
    // ATTACK ENEMY
    AddStep([this, game, isoMap]
    {
        const core::Pointd2D p0(1200, 450);
        const GameObject * tower = GetObjectInCell(cellDT3);
        return new StepGameObjectAttackSimple(game, tower, isoMap, cellEnemy2, p0);
    });
    // WAIT ENEMY DESTROYED
    AddStep([this]
    {
        GameObject * enemy = GetObjectInCell(cellEnemy2);
        return new StepGameWaitEnemyKilled(enemy, GetGameMap());
    });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([]
    {
        const core::Pointd2D p0(1150, 450);
        return new StepGameSingleInfo(p0, "TUT_FATAL_SHOT");
    });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT UNIT TO CONTINUE
    AddStep([local, game, isoMap]
    {
        const auto unit = local->GetUnit(indUnit2);
        const core::Pointd2D p0(1300, 450);
        return new StepGameUnit(game, isoMap, unit, p0);
    });
    // INTRO WALL BUILDING
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([]
    {
        const core::Pointd2D p0(1200, 400);
        return new StepGameWallBuildIntro(p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE UNIT NEXT TO BUILD CELL
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local, isoMap, game]
    {
        const auto unit = local->GetUnit(indUnit2);
        const Cell2D target(51, 33);
        const core::Pointd2D p0(1100, 400);
        return new StepGameMoveUnitSimple(game, unit, isoMap, target, p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // BUILD GATE WITH UNIT 2
    AddStep([game, gs, panelActions]
        {
            const core::Pointd2D p0(1100, 400);
            return new StepGameBuildStructIntro(game, gs, panelActions, "TUT_GAME_BUILD_GATE_1", p0);
        });
    AddStep([hud]
        {
            return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                              "TUT_GAME_BUILD_GATE_2",
                                              "TUT_GAME_BASE_BUILD_UNIT_3",
                                              TutorialConstants::catDefenses,
                                              TutorialConstants::structGate);
        });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit2);
            const core::Pointd2D p0(1100, 450);
            const Cell2D target(51, 34);
            return new StepGameBuildTowerEnd(isoMap, unit, target, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL WITH UNIT 2
    AddStep([game, gs, panelActions] { return new StepGameWallBuildIcon(game, gs, panelActions); });
    AddStep([this, isoMap, game]
        {
            const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
            const Cell2D target(50, 34);
            return new StepGameWallBuildStart(game, isoMap, cellStart, target);
        });
    AddStep([isoMap, local, game]
        {
            const auto unit = local->GetUnit(indUnit2);
            const Cell2D cellEnd(48, 34);
            const core::Pointd2D p0(900, 150);
            return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
        });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE VIEW BACK TO BASE
    AddStep([panelTurn]
        {
            const core::Pointd2D p0(250, 600);
            return new StepGameBackToBase(panelTurn, "TUT_GAME_BACK_TO_BASE_1c", p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // COLLECT PRIMARY MISSION GOAL
    AddStep([localBase, game, isoMap]
        {
            const core::Pointd2D p0(500, 200);
            return new StepGameSelectObject(game, isoMap, localBase, "TUT_GAME_BASE_4", p0);
        });
    AddStep([game, gs, panelActions] { return new StepGameMissionGoalsIcon(game, gs, panelActions, false); });
    AddStep([hud] { return new StepGamePrimaryMissionGoal(hud); });
}

TutorialGame1::~TutorialGame1()
{
    // clear perfect shot flag from defensive tower 1
    GameObject * tower = GetObjectInCell(cellDT1);

    if(tower != nullptr && tower->GetObjectType() == ObjectData::TYPE_DEFENSIVE_TOWER)
    {
        tower->SetPerfectShotEnabled(false, false);
        tower->SetFatalHitEnabled(true);
    }

    // clear perfect shot flag from defensive tower 3
    tower = GetObjectInCell(cellDT3);

    if(tower != nullptr && tower->GetObjectType() == ObjectData::TYPE_DEFENSIVE_TOWER)
        tower->SetPerfectShotEnabled(false, false);

    // make sure selection is allowed again
    GetScreen()->SetSelectionAllowed(true);
}

} // namespace game
