#include "Tutorial/TutorialGame1.h"

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
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepAISetActive.h"
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
#include "Tutorial/StepGameMoveCameraOverObject.h"
#include "Tutorial/StepGameMoveToTower.h"
#include "Tutorial/StepGameMoveUnit.h"
#include "Tutorial/StepGameMoveUnitToCorner.h"
#include "Tutorial/StepGamePrimaryMissionGoal.h"
#include "Tutorial/StepGameQuickUnitButton.h"
#include "Tutorial/StepGameSelectObject.h"
#include "Tutorial/StepGameSetObjectMaxHealth.h"
#include "Tutorial/StepGameSetObjectPerfectShot.h"
#include "Tutorial/StepGameSetSelectionActiveAction.h"
#include "Tutorial/StepGameSetSelectionDefaultAction.h"
#include "Tutorial/StepGameStructConnected.h"
#include "Tutorial/StepGameStructDisconnected.h"
#include "Tutorial/StepGameTestCameraFocus.h"
#include "Tutorial/StepGameTowerIntro.h"
#include "Tutorial/StepGameTurnEnergy.h"
#include "Tutorial/StepGameUnit.h"
#include "Tutorial/StepGameUnitConquerCellsIcon.h"
#include "Tutorial/StepGameUpgradeIntro.h"
#include "Tutorial/StepGameUpgradeUnit.h"
#include "Tutorial/StepGameUpgradeUnitFree.h"
#include "Tutorial/StepGameWaitEnemyKilled.h"
#include "Tutorial/StepGameWaitTurn.h"
#include "Tutorial/StepGameWallBuildEnd.h"
#include "Tutorial/StepGameWallBuildIcon.h"
#include "Tutorial/StepGameWallBuildIntro.h"
#include "Tutorial/StepGameWallBuildStart.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/GameHUD.h"
#include "Widgets/PanelObjectActions.h"

namespace
{
using namespace game;

const Cell2D cellDT1(17, 16);
const Cell2D cellDT2(37, 24);
const Cell2D cellEnemy(17, 18);
const Cell2D cellMatGen1(22, 10);
const Cell2D cellMatGen2(42, 20);
const Cell2D cellEneGen2(31, 26);

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
    AddStep([] { return new StepGameIntro; });
    AddStep([] { return new StepDelay(0.5f); });
    // BASE INTRO
    AddStep([game, isoMap, localBase] { return new StepGameBase(game, isoMap, localBase); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD NEW UNIT
    AddStep([panelActions] { return new StepGameBaseBuildUnitIcon(panelActions); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([hud] { return new StepGameBaseBuildUnit(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([localBase] { return new StepDelay(localBase->GetTimeBuildUnit()); });
    // MOVE UNIT NEXT TO ENERGY GENERATOR
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indUnit1);
                return new StepGameUnit(game, isoMap, unit);
            });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepDelay(0.5f); });

    AddStep([game, isoMap, local] { return new StepGameMoveUnit(game, local, isoMap); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    // CONQUER ENERGY GENERATOR
    AddStep([this, local, isoMap, game]
            {
                const Cell2D cellGen(31, 11);
                const GameObject * gen = GetObjectInCell(cellGen);

                return new StepGameConquerStruct(game, local, gen, isoMap);
            });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::IDLE); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepDelay(0.5f); });
    // EXPLAIN ENERGY AND END TURN
    AddStep([hud] { return new StepGameTurnEnergy(hud); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurn(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([] { return new StepGameEnergyRegeneration; });
    // EXPLAIN STRUCTURE CONNECTIONS AND CONNECT GENERATOR TO BASE
    AddStep([] { return new StepGameStructDisconnected; });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
        {
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            return new StepGameConquerCells(game, isoMap, cellStart);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit1);
            const Cell2D cellEnd(38, 10);
            const core::Pointd2D p0(1150, 500);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([] { return new StepGameStructConnected; });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER MATERIAL GENERATOR
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
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::MOVE); });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit1);
            const GameObject * gen = GetObjectInCell(cellMatGen1);
            const core::Pointd2D p0(1300, 400);
            return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
        });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::IDLE); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepGameEndTurnIntro(); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT MATERIAL GENERATOR
    AddStep([]
        {
            const core::Pointd2D p0(1250, 200);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1200, 400);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(23, 10);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit1);
            const Cell2D cellEnd(29, 10);
            const core::Pointd2D p0(1150, 500);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::MOVE); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE UNIT TO VISIBLE CORNER
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indUnit1);
                const Cell2D target(17, 15);
                const core::Pointd2D p0(1000, 100);
                return new StepGameMoveUnitToCorner(unit, isoMap, target, p0);
            });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::IDLE); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepDelay(0.5f); });
    // UPGRADE UNIT
    AddStep([panelActions]
        {
            core::Pointd2D p0(900, 250);
            return new StepGameUpgradeIntro(panelActions, "TUT_GAME_UPGRADE_1", p0);
        });
    AddStep([hud] { return new StepGameUpgradeUnit(hud, true); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD DEFENSIVE TOWER 1
    AddStep([panelActions]
        {
            const core::Pointd2D p0(900, 250);

            return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
        });
    AddStep([hud]
            {
                const int indCat = 1;
                const int indStruct = 1;
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_DTOWER_4", indCat, indStruct);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
        {
            const auto unit = local->GetUnit(indUnit1);
            const core::Pointd2D p0(900, 250);
            return new StepGameBuildTowerEnd(isoMap, unit, cellDT1, p0);
        });
    AddStep([this]
            {
                GameObject * tower = GetObjectInCell(cellDT1);
                return new StepGameSetObjectPerfectShot(tower, true);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT DEFENSIVE TOWER
    AddStep([game]
        {
            const core::Pointd2D p0(1000, 150);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
        {
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(17, 15);
            const core::Pointd2D p0(1000, 250);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit1);
            const Cell2D cellEnd(21, 11);
            const core::Pointd2D p0(1000, 300);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL FROM TOWER
    AddStep([] { return new StepGameWallBuildIntro(); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelActions] { return new StepGameWallBuildIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
                const Cell2D target(17, 15);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([isoMap, local, game]
            {
                const auto unit = local->GetUnit(indUnit1);
                const Cell2D cellEnd(14, 15);
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
    AddStep([] { return new StepDelay(0.5f); });
    // ===== PART 2 =====
    // BUILD SECOND UNIT
    AddStep([localBase, game, isoMap]
            {
                const core::Pointd2D p0(500, 200);
                return new StepGameSelectObject(game, isoMap, localBase, "TUT_GAME_BASE_4", p0);
            });
    AddStep([panelActions]
            {
                return new StepGameBuildUnitStart(panelActions,
                                                  PanelObjectActions::BTN_BUILD_UNIT_BASE);
            });
    AddStep([hud] { return new StepGameBuildUnitEnd(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([localBase] { return new StepDelay(localBase->GetTimeBuildUnit()); });
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indUnit2);
                return new StepGameUnit(game, isoMap, unit);
            });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::IDLE); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    // CONQUER SECOND MATERIAL GENERATOR
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local]
        {
            const GameObject * gen = GetObjectInCell(cellMatGen2);

            return new StepGameConquerMaterialGenIntro(gen);
        });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::MOVE); });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit2);
            const GameObject * gen = GetObjectInCell(cellMatGen2);
            const core::Pointd2D p0(1250, 550);
            return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
        });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT SECOND MATERIAL GENERATOR
    AddStep([]
            {
                const core::Pointd2D p0(1150, 500);
                return new StepGameConnectStructIntro(p0);
            });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(200, 550);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(41, 18);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit2);
            const core::Pointd2D p0(700, 250);
            const Cell2D cellEnd(41, 11);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::MOVE); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE UNIT TO VISIBLE CORNER
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indUnit2);
                const Cell2D target(37, 23);
                const core::Pointd2D p0(1050, 300);
                return new StepGameMoveUnitToCorner(unit, isoMap, target, p0);
            });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::IDLE); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD DEFENSIVE TOWER
    AddStep([panelActions]
            {
                const core::Pointd2D p0(900, 250);

                return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
            });
    AddStep([hud]
            {
                const int indCat = 1;
                const int indStruct = 1;
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_DTOWER_4", indCat, indStruct);
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
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // // CONNECT DEFENSIVE TOWERs
    AddStep([]
        {
            const core::Pointd2D p0(1100, 450);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
        {
            const core::Pointd2D p0(1100, 450);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(37, 23);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit2);
            const Cell2D cellEnd(40, 20);
            const core::Pointd2D p0(1100, 450);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
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
                                            ObjectData::TYPE_UNIT_SOLDIER1, cellEnemy, true);
            });
    AddStep([this]
            {
                const float maxHealth = 80.f;
                GameObject * enemy = GetObjectInCell(cellEnemy);

                return new StepGameSetObjectMaxHealth(enemy, maxHealth);
            });
    AddStep([gs] { return new StepGameClearSelection(gs); });
    // EXPLAIN CAMERA MOVE AND MOVE TO TOWER 1
    AddStep([] { return new StepGameMapNavigation; });
    AddStep([] { return new StepGameMoveToTower; });
    AddStep([this] { return new StepGameEnableCamera(GetCameraMapController()); });
    AddStep([this, local]
            {
                const GameObject * tower = GetObjectInCell(cellDT1);
                const IsoObject * obj = tower->GetIsoObject();
                const int areaHalfW = 300;
                const int areaHalfH = 200;
                const int tlX = obj->GetX() - areaHalfW;
                const int tlY = obj->GetY() - areaHalfH;
                const int brX = obj->GetX() + areaHalfW;
                const int brY = obj->GetY() + areaHalfH;

                const auto cam = GetCameraMapController()->GetCamera();

                return new StepGameTestCameraFocus(cam, tlX, tlY, brX, brY);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([this]
            {
                const GameObject * tower = GetObjectInCell(cellDT1);

                return new StepGameMoveCameraOverObject(tower);
            });
    // ENEMY KILL
    AddStep([] { return new StepGameTowerIntro; });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([this]
            {
                GameObject * enemy = GetObjectInCell(cellEnemy);
                return new StepGameWaitEnemyKilled(enemy, GetGameMap());
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this] { return new StepGameEnemyKilled(); });
    AddStep([hud] { return new StepGameQuickUnitButton(hud, indUnit2); });
    // ===== PART 4 =====
    // TODO remove next 2 steps when trying full tutorial
    // AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    // AddStep([this] { return new StepGameWaitTurn(gs); });
    // BUILD WALL FROM TOWER
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([] { return new StepGameWallBuildIntro(); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelActions] { return new StepGameWallBuildIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
                const Cell2D target(37, 25);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indUnit2);
                const Cell2D cellEnd(37, 28);
                const core::Pointd2D p0(900, 250);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // UPGRADE UNIT
    AddStep([panelActions, game]
        {
            core::Pointd2D p0(1000, 550);
            return new StepGameUpgradeIntro(panelActions, "TUT_GAME_UPGRADE_1b", p0);
        });
    AddStep([hud] { return new StepGameUpgradeUnitFree(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER SECOND ENERGY GENERATOR
    AddStep([this, local, game]
        {
            const GameObject * gen = GetObjectInCell(cellEneGen2);
            const core::Pointd2D p0(400, 200);
            return new StepGameConquerEnergyGenIntro(gen, p0);
        });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::MOVE); });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit2);
            const GameObject * gen = GetObjectInCell(cellEneGen2);
            const core::Pointd2D p0(400, 200);
            return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
        });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
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
    AddStep([panelActions] { return new StepGameMissionGoalsIcon(panelActions, true); });
    AddStep([hud] { return new StepGameMissionGoalsDialog(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // BACK TO UNIT 2
    AddStep([hud] { return new StepGameQuickUnitButton(hud, indUnit2); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT SECOND ENERGY GENERATOR
    AddStep([]
            {
                const core::Pointd2D p0(1250, 350);
                return new StepGameConnectStructIntro(p0);
            });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(1100, 350);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(32, 26);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indUnit2);
                const core::Pointd2D p0(1000, 550);
                const Cell2D cellEnd(36, 26);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE VIEW BACK TO BASE
    AddStep([panelTurn]
            {
                const core::Pointd2D p0(800, 500);
                return new StepGameBackToBase(panelTurn, "TUT_GAME_BACK_TO_BASE_1c", p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // COLLECT PRIMARY MISSION GOAL
    AddStep([localBase, game, isoMap]
            {
                const core::Pointd2D p0(500, 200);
                return new StepGameSelectObject(game, isoMap, localBase, "TUT_GAME_BASE_4", p0);
            });
    AddStep([panelActions] { return new StepGameMissionGoalsIcon(panelActions, false); });
    AddStep([hud] { return new StepGamePrimaryMissionGoal(hud); });
}

TutorialGame1::~TutorialGame1()
{
    // clear perfect shot flag from defensive tower
    GameObject * tower = GetObjectInCell(cellDT1);

    if(tower != nullptr && tower->GetObjectType() == ObjectData::TYPE_DEFENSIVE_TOWER)
        tower->SetPerfectShot(false);
}

} // namespace game
