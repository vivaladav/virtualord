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
#include "Tutorial/StepGameBaseBuildUnitEnd.h"
#include "Tutorial/StepGameBaseBuildUnitIcon.h"
#include "Tutorial/StepGameBaseBuildUnitStart.h"
#include "Tutorial/StepGameBuildTower.h"
#include "Tutorial/StepGameBuildTowerEnd.h"
#include "Tutorial/StepGameBuildTowerIntro.h"
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
#include "Tutorial/StepGameSelectBase.h"
#include "Tutorial/StepGameSetObjectAttackMode.h"
#include "Tutorial/StepGameSetObjectMaxHealth.h"
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
#include "Tutorial/StepGameWaitEnemyKilled.h"
#include "Tutorial/StepGameWaitTurn.h"
#include "Tutorial/StepGameWallBuildEnd.h"
#include "Tutorial/StepGameWallBuildIcon.h"
#include "Tutorial/StepGameWallBuildIntro.h"
#include "Tutorial/StepGameWallBuildStart.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/GameHUD.h"

namespace
{
using namespace game;

const Cell2D cellDT1(17, 16);
const Cell2D cellDT2(37, 26);
const Cell2D cellEnemy(17, 18);
const Cell2D cellMatGen1(22, 10);
const Cell2D cellMatGen2(42, 21);
const Cell2D cellEneGen2(29, 26);

constexpr unsigned int indUnit1 = 0;
// TODO change this back to 1 when testing full tutorial
constexpr unsigned int indUnit2 = 1;

}

namespace game
{

TutorialGame1::TutorialGame1(Screen * screen)
    : TutorialGame(screen, TUTORIAL_MISSION_1)
{
    auto gs = GetScreen();
    auto game = gs->GetGame();
    auto hud = gs->GetHUD();
    auto isoMap = GetIsoMap();

    const Player * local = game->GetPlayerByIndex(0);
    Player * playerAI = game->GetPlayerByIndex(1);

    auto panelActions = hud->GetPanelObjectActions();
    auto panelObj = hud->GetPanelSelectedObject();
    auto panelTurn = hud->GetPanelTurnControl();
    auto localBase = local->GetBase();

    // ===== SETUP =====
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    // make AI idle for now
    AddStep([playerAI] { return new StepAISetActive(playerAI->GetAI(), false); });
    AddStep([] { return new StepDelay(1.f); });

    // ===== PART 1 =====
    // INTRO
    AddStep([] { return new StepGameIntro; });
    AddStep([] { return new StepDelay(0.3f); });
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
     AddStep([] { return new StepGameMoveCamera(450, -150, 300.f); });
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
    AddStep([] { return new StepDelay(1.0f); });
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
            const sgl::core::Pointd2D p0(1150, 500);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([] { return new StepGameStructConnected; });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER MATERIAL GENERATOR
    AddStep([this, local]
        {
            const GameObject * gen = GetObjectInCell(cellMatGen1);

            return new StepGameMaterialGenerator(gen);
        });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(1.0f); });
    AddStep([] { return new StepGameMoveCamera(300, -150); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::MOVE); });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit1);
            const GameObject * gen = GetObjectInCell(cellMatGen1);
            const sgl::core::Pointd2D p0(1300, 200);

            return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
        });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::IDLE); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepGameEndTurnIntro(); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(1.0f); });
    // CONNECT MATERIAL GENERATOR
    AddStep([]
        {
            const sgl::core::Pointd2D p0(1250, 200);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
        {
            const sgl::core::Pointd2D p0(1250, 250);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(23, 10);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit1);
            const Cell2D cellEnd(29, 10);
            const sgl::core::Pointd2D p0(1150, 500);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::MOVE); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([] { return new StepDelay(1.0f); });
    // MOVE UNIT TO VISIBLE CORNER
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indUnit1);
                const Cell2D target(17, 15);
                const sgl::core::Pointd2D p0(1000, 100);
                return new StepGameMoveUnitToCorner(unit, isoMap, target, p0);
            });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::IDLE); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([] { return new StepGameMoveCamera(400, -100); });
    // UPGRADE UNIT
    AddStep([panelActions] { return new StepGameUpgradeIntro(panelActions, "TUT_GAME_UPGRADE_1"); });
    AddStep([hud] { return new StepGameUpgradeUnit(hud, true); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(1.0f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([] { return new StepDelay(1.0f); });
    // BUILD DEFENSIVE TOWER 1
    AddStep([panelActions] { return new StepGameBuildTowerIntro(panelActions); });
    AddStep([hud] { return new StepGameBuildTower(hud); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
        {
            const auto unit = local->GetUnit(indUnit1);
            return new StepGameBuildTowerEnd(isoMap, unit, cellDT1);
        });
    AddStep([this]
            {
                GameObject * tower = GetObjectInCell(cellDT1);
                return new StepGameSetObjectAttackMode(tower, ATT_PERFECT_SHOT);
            });
    AddStep([] { return new StepDelay(1.0f); });
    // CONNECT DEFENSIVE TOWER
    AddStep([]
        {
            const sgl::core::Pointd2D p0(1250, 200);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
        {
            const sgl::core::Pointd2D p0(1250, 250);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(17, 15);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit1);
            const Cell2D cellEnd(21, 11);
            const sgl::core::Pointd2D p0(1250, 300);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([] { return new StepDelay(1.0f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([] { return new StepDelay(1.0f); });
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
                const sgl::core::Pointd2D p0(900, 150);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE VIEW BACK TO BASE
    AddStep([panelTurn]
    {
        const sgl::core::Pointd2D p0(800, 600);
        return new StepGameBackToBase(panelTurn, "TUT_GAME_BACK_TO_BASE_1", p0);
    });
    AddStep([] { return new StepDelay(1.0f); });
    // ===== PART 2 =====
    // BUILD SECOND UNIT
    AddStep([localBase, game, isoMap] { return new StepGameSelectBase(game, isoMap, localBase); });
    AddStep([panelActions] { return new StepGameBaseBuildUnitStart(panelActions); });
    AddStep([hud] { return new StepGameBaseBuildUnitEnd(hud); });
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
    AddStep([] { return new StepGameMoveCamera(500, 200); });
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
            const sgl::core::Pointd2D p0(1150, 500);

            return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
        });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT SECOND MATERIAL GENERATOR
    AddStep([]
            {
                const sgl::core::Pointd2D p0(1150, 500);
                return new StepGameConnectStructIntro(p0);
            });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
        {
            const sgl::core::Pointd2D p0(200, 550);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(41, 19);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit2);
            const sgl::core::Pointd2D p0(700, 250);
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
                const Cell2D target(37, 25);
                const sgl::core::Pointd2D p0(1050, 300);
                return new StepGameMoveUnitToCorner(unit, isoMap, target, p0);
            });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::IDLE); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([] { return new StepGameMoveCamera(400, 100); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD DEFENSIVE TOWER
    AddStep([panelActions] { return new StepGameBuildTowerIntro(panelActions); });
    AddStep([hud] { return new StepGameBuildTower(hud); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
        {
            const auto unit = local->GetUnit(indUnit2);
            return new StepGameBuildTowerEnd(isoMap, unit, cellDT2);
        });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // // CONNECT DEFENSIVE TOWERs
    AddStep([]
        {
            const sgl::core::Pointd2D p0(1100, 450);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
        {
            const sgl::core::Pointd2D p0(1100, 450);
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
            const Cell2D target(37, 25);
            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indUnit2);
            const Cell2D cellEnd(40, 21);
            const sgl::core::Pointd2D p0(1100, 450);
            return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // ===== PART 3 =====
    // ADD ENEMY NEAR TOWER 1
    AddStep([]
            {
                const sgl::core::Pointd2D p0(1100, 350);
                return new StepGameEnemyIntro(p0);
            });
    AddStep([this, playerAI]
            {
                return new StepGameAddEnemy(GetGameMap(), playerAI,
                                            ObjectData::TYPE_UNIT_SOLDIER1, cellEnemy, true);
            });
    AddStep([this]
            {
                const float maxHealth = 50.f;
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
                const int areaHalfW = 360;
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
    AddStep([] { return new StepDelay(1.0f); });
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
                const Cell2D target(37, 27);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indUnit2);
                const Cell2D cellEnd(37, 30);
                const sgl::core::Pointd2D p0(900, 250);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([] { return new StepGameMoveCamera(500, 0, 600.f); });
    // UPGRADE UNIT
    AddStep([panelActions] { return new StepGameUpgradeIntro(panelActions, "TUT_GAME_UPGRADE_1b"); });
    AddStep([hud] { return new StepGameUpgradeUnit(hud, false); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER SECOND ENERGY GENERATOR
    AddStep([this, local]
            {
                const GameObject * gen = GetObjectInCell(cellEneGen2);

                return new StepGameConquerEnergyGenIntro(gen);
            });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::MOVE); });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indUnit2);
                const GameObject * gen = GetObjectInCell(cellEneGen2);
                const sgl::core::Pointd2D p0(550, 250);

                return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
            });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE VIEW BACK TO BASE
    AddStep([panelTurn]
            {
                const sgl::core::Pointd2D p0(800, 550);
                return new StepGameBackToBase(panelTurn, "TUT_GAME_BACK_TO_BASE_1b", p0);
            });
    AddStep([] { return new StepDelay(1.0f); });
    // ===== PART 5 =====
    // INTRODUCE MISSION GOALS
    AddStep([localBase, game, isoMap] { return new StepGameSelectBase(game, isoMap, localBase); });
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
                const sgl::core::Pointd2D p0(1250, 350);
                return new StepGameConnectStructIntro(p0);
            });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const sgl::core::Pointd2D p0(1100, 350);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(30, 26);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indUnit2);
                const sgl::core::Pointd2D p0(1000, 550);
                const Cell2D cellEnd(36, 26);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE VIEW BACK TO BASE
    AddStep([panelTurn]
            {
                const sgl::core::Pointd2D p0(800, 500);
                return new StepGameBackToBase(panelTurn, "TUT_GAME_BACK_TO_BASE_1c", p0);
            });
    AddStep([] { return new StepDelay(1.0f); });
    // COLLECT PRIMARY MISSION GOAL
    AddStep([localBase, game, isoMap] { return new StepGameSelectBase(game, isoMap, localBase); });
    AddStep([panelActions] { return new StepGameMissionGoalsIcon(panelActions, false); });
    AddStep([hud] { return new StepGamePrimaryMissionGoal(hud); });
}

} // namespace game
