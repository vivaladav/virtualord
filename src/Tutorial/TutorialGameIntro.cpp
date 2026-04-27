#include "Tutorial/TutorialGameIntro.h"

#include "CameraMapController.h"
#include "Game.h"
#include "GameMap.h"
#include "IsoObject.h"
#include "Player.h"
#include "GameObjects/Base.h"
#include "GameObjects/ObjectData.h"
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
#include "Tutorial/StepGameBaseFeatures.h"
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

#include <cassert>

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

TutorialGameIntro::TutorialGameIntro(Screen * screen)
    : Tutorial(TUTORIAL_MISSION_INTRO)
    , mScreen(dynamic_cast<ScreenGame *>(screen))
{
    assert(mScreen);

    auto game = mScreen->GetGame();

    const Player * local = game->GetPlayerByIndex(0);
    Player * playerAI = game->GetPlayerByIndex(1);

    auto panelActions = mScreen->mHUD->GetPanelObjectActions();
    auto panelObj = mScreen->mHUD->GetPanelSelectedObject();
    auto panelTurn = mScreen->mHUD->GetPanelTurnControl();
    auto localBase = local->GetBase();

    AddStep([this] { return new StepGameDisableCamera(mScreen->mCamController); });
    // make AI idle for now
    AddStep([playerAI] { return new StepAISetActive(playerAI->GetAI(), false); });
    AddStep([] { return new StepDelay(1.f); });

    // ===== PART 1 =====
    // INTRO
    AddStep([] { return new StepGameIntro; });
    AddStep([] { return new StepDelay(0.3f); });
    // BASE INTRO
    AddStep([localBase] { return new StepGameBase(localBase); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelActions, panelObj] { return new StepGameBaseFeatures(panelObj, panelActions); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD NEW UNIT
    AddStep([panelActions] { return new StepGameBaseBuildUnitIcon(panelActions); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this] { return new StepGameBaseBuildUnit(mScreen->mHUD); });
    AddStep([this] { return new StepGameDisableCamera(mScreen->mCamController); });
    AddStep([localBase] { return new StepDelay(localBase->GetTimeBuildUnit()); });
    // MOVE UNIT NEXT TO ENERGY GENERATOR
    AddStep([local]
            {
                const auto unit = local->GetUnit(indUnit1);
                return new StepGameUnit(unit);
            });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepDelay(0.5f); });

    AddStep([this, local] { return new StepGameMoveUnit(local, mScreen->mIsoMap); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
     AddStep([] { return new StepGameMoveCamera(450, -150); });
    // CONQUER ENERGY GENERATOR
    AddStep([this, local]
            {
                const Cell2D cellGen(31, 11);
                const GameObject * gen = GetObjectInCell(cellGen);

                return new StepGameConquerStruct(local, gen, mScreen->mIsoMap);
            });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::IDLE); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepDelay(0.5f); });
    // EXPLAIN ENERGY AND END TURN
    AddStep([this] { return new StepGameTurnEnergy(mScreen->mHUD); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurn(panelTurn); });
    AddStep([this] { return new StepGameWaitTurn(mScreen); });
    AddStep([] { return new StepDelay(1.0f); });
    AddStep([] { return new StepGameEnergyRegeneration; });
    // EXPLAIN STRUCTURE CONNECTIONS AND CONNECT GENERATOR TO BASE
    AddStep([] { return new StepGameStructDisconnected; });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this]
        {
            const Cell2D & cellStart = mScreen->mOverlayCellConquest->GetCellStart();
            return new StepGameConquerCells(mScreen->mIsoMap, cellStart);
        });
    AddStep([this, local]
        {
            const auto unit = local->GetUnit(indUnit1);
            const Cell2D cellEnd(38, 10);
            const sgl::core::Pointd2D p0(1250, 300);
            return new StepGameConquerCellsEnd(mScreen->mIsoMap, unit, cellEnd, p0);
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
    AddStep([this] { return new StepGameWaitTurn(mScreen); });
    AddStep([] { return new StepDelay(1.0f); });
    AddStep([] { return new StepGameMoveCamera(300, -150); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::MOVE); });
    AddStep([this, local]
        {
            const auto unit = local->GetUnit(indUnit1);
            const GameObject * gen = GetObjectInCell(cellMatGen1);
            const sgl::core::Pointd2D p0(1300, 200);

            return new StepGameConquerStructSimple(unit, gen, mScreen->mIsoMap, p0);
        });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::IDLE); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepGameEndTurnIntro(); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([this] { return new StepGameWaitTurn(mScreen); });
    AddStep([] { return new StepDelay(1.0f); });
    // CONNECT MATERIAL GENERATOR
    AddStep([]
        {
            const sgl::core::Pointd2D p0(1250, 200);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this]
        {
            const sgl::core::Pointd2D p0(1250, 250);
            const Cell2D & cellStart = mScreen->mOverlayCellConquest->GetCellStart();
            const Cell2D target(23, 10);
            return new StepGameConquerCellsSimple(mScreen->mIsoMap, cellStart, target, p0);
        });
    AddStep([this, local]
        {
            const auto unit = local->GetUnit(indUnit1);
            const Cell2D cellEnd(29, 10);
            const sgl::core::Pointd2D p0(1250, 300);
            return new StepGameConquerCellsEnd(mScreen->mIsoMap, unit, cellEnd, p0);
        });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::MOVE); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([] { return new StepDelay(1.0f); });
    // MOVE UNIT TO VISIBLE CORNER
    AddStep([this, local]
            {
                const auto unit = local->GetUnit(indUnit1);
                const Cell2D target(17, 15);
                const sgl::core::Pointd2D p0(550, 100);
                return new StepGameMoveUnitToCorner(unit, mScreen->mIsoMap, target, p0);
            });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::IDLE); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([] { return new StepGameMoveCamera(400, -100); });
    // UPGRADE UNIT
    AddStep([panelActions] { return new StepGameUpgradeIntro(panelActions, "TUT_GAME_UPGRADE_1"); });
    AddStep([this] { return new StepGameUpgradeUnit(mScreen->mHUD, true); });
    AddStep([this] { return new StepGameDisableCamera(mScreen->mCamController); });
    AddStep([] { return new StepDelay(1.0f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([] { return new StepDelay(1.0f); });
    // BUILD DEFENSIVE TOWER 1
    AddStep([panelActions] { return new StepGameBuildTowerIntro(panelActions); });
    AddStep([this] { return new StepGameBuildTower(mScreen->mHUD); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local]
        {
            const auto unit = local->GetUnit(indUnit1);
            return new StepGameBuildTowerEnd(mScreen->mIsoMap, unit, cellDT1);
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
    AddStep([this]
        {
            const sgl::core::Pointd2D p0(1250, 250);
            const Cell2D & cellStart = mScreen->mOverlayCellConquest->GetCellStart();
            const Cell2D target(17, 15);
            return new StepGameConquerCellsSimple(mScreen->mIsoMap, cellStart, target, p0);
        });
    AddStep([this, local]
        {
            const auto unit = local->GetUnit(indUnit1);
            const Cell2D cellEnd(21, 11);
            const sgl::core::Pointd2D p0(1250, 300);
            return new StepGameConquerCellsEnd(mScreen->mIsoMap, unit, cellEnd, p0);
        });
    AddStep([] { return new StepDelay(1.0f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([] { return new StepDelay(1.0f); });
    // BUILD WALL FROM TOWER
    AddStep([] { return new StepGameWallBuildIntro(); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelActions] { return new StepGameWallBuildIcon(panelActions); });
    AddStep([this]
            {
                const Cell2D & cellStart = mScreen->mOverlayWall->GetCellStart();
                const Cell2D target(17, 15);
                return new StepGameWallBuildStart(mScreen->mIsoMap, cellStart, target);
            });
    AddStep([this, local]
            {
                const auto unit = local->GetUnit(indUnit1);
                const Cell2D cellEnd(14, 15);
                return new StepGameWallBuildEnd(mScreen->mIsoMap, unit, cellEnd);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE VIEW BACK TO BASE
    AddStep([panelTurn] { return new StepGameBackToBase(panelTurn, "TUT_GAME_BACK_TO_BASE_1"); });
    AddStep([] { return new StepDelay(1.0f); });
    // ===== PART 2 =====
    // BUILD SECOND UNIT
    AddStep([localBase] { return new StepGameSelectBase(localBase); });
    AddStep([panelActions] { return new StepGameBaseBuildUnitStart(panelActions); });
    AddStep([this] { return new StepGameBaseBuildUnitEnd(mScreen->mHUD); });
    AddStep([this] { return new StepGameDisableCamera(mScreen->mCamController); });
    AddStep([localBase] { return new StepDelay(localBase->GetTimeBuildUnit()); });
    AddStep([local]
            {
                const auto unit = local->GetUnit(indUnit2);
                return new StepGameUnit(unit);
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
    AddStep([this, local]
        {
            const auto unit = local->GetUnit(indUnit2);
            const GameObject * gen = GetObjectInCell(cellMatGen2);
            const sgl::core::Pointd2D p0(1150, 450);

            return new StepGameConquerStructSimple(unit, gen, mScreen->mIsoMap, p0);
        });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([this] { return new StepGameWaitTurn(mScreen); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT SECOND MATERIAL GENERATOR
    AddStep([]
            {
                const sgl::core::Pointd2D p0(1150, 500);
                return new StepGameConnectStructIntro(p0);
            });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this]
        {
            const sgl::core::Pointd2D p0(200, 550);
            const Cell2D & cellStart = mScreen->mOverlayCellConquest->GetCellStart();
            const Cell2D target(41, 19);
            return new StepGameConquerCellsSimple(mScreen->mIsoMap, cellStart, target, p0);
        });
    AddStep([this, local]
        {
            const auto unit = local->GetUnit(indUnit2);
            const sgl::core::Pointd2D p0(700, 250);
            const Cell2D cellEnd(41, 11);
            return new StepGameConquerCellsEnd(mScreen->mIsoMap, unit, cellEnd, p0);
        });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::MOVE); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([this] { return new StepGameWaitTurn(mScreen); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE UNIT TO VISIBLE CORNER
    AddStep([this, local]
            {
                const auto unit = local->GetUnit(indUnit2);
                const Cell2D target(37, 25);
                const sgl::core::Pointd2D p0(1050, 300);
                return new StepGameMoveUnitToCorner(unit, mScreen->mIsoMap, target, p0);
            });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::IDLE); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([] { return new StepGameMoveCamera(400, 100); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([this] { return new StepGameWaitTurn(mScreen); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD DEFENSIVE TOWER
    AddStep([panelActions] { return new StepGameBuildTowerIntro(panelActions); });
    AddStep([this] { return new StepGameBuildTower(mScreen->mHUD); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local]
        {
            const auto unit = local->GetUnit(indUnit2);
            return new StepGameBuildTowerEnd(mScreen->mIsoMap, unit, cellDT2);
        });
    AddStep([this] { return new StepGameDisableCamera(mScreen->mCamController); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([this] { return new StepGameWaitTurn(mScreen); });
    AddStep([] { return new StepDelay(0.5f); });
    // // CONNECT DEFENSIVE TOWERs
    AddStep([]
        {
            const sgl::core::Pointd2D p0(1100, 450);
            return new StepGameConnectStructIntro(p0);
        });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this]
        {
            const sgl::core::Pointd2D p0(1100, 450);
            const Cell2D & cellStart = mScreen->mOverlayCellConquest->GetCellStart();
            const Cell2D target(37, 25);
            return new StepGameConquerCellsSimple(mScreen->mIsoMap, cellStart, target, p0);
        });
    AddStep([this, local]
        {
            const auto unit = local->GetUnit(indUnit2);
            const Cell2D cellEnd(40, 21);
            const sgl::core::Pointd2D p0(1100, 450);
            return new StepGameConquerCellsEnd(mScreen->mIsoMap, unit, cellEnd, p0);
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
                return new StepGameAddEnemy(mScreen->mGameMap, playerAI,
                                            ObjectData::TYPE_UNIT_SOLDIER1, cellEnemy, true);
            });
    AddStep([this]
            {
                const float maxHealth = 50.f;
                GameObject * enemy = GetObjectInCell(cellEnemy);

                return new StepGameSetObjectMaxHealth(enemy, maxHealth);
            });
    AddStep([this] { return new StepGameClearSelection(mScreen); });
    // EXPLAIN CAMERA MOVE AND MOVE TO TOWER 1
    AddStep([] { return new StepGameMapNavigation; });
    AddStep([] { return new StepGameMoveToTower; });
    AddStep([this] { return new StepGameEnableCamera(mScreen->mCamController); });
    AddStep([this, local]
            {
                const GameObject * tower = GetObjectInCell(cellDT1);
                const IsoObject * obj = tower->GetIsoObject();
                const int areaHalfW = 540;
                const int areaHalfH = 270;
                const int tlX = obj->GetX() - areaHalfW;
                const int tlY = obj->GetY() - areaHalfH;
                const int brX = obj->GetX() + areaHalfW;
                const int brY = obj->GetY() + areaHalfH;

                const auto cam = mScreen->mCamController->GetCamera();

                return new StepGameTestCameraFocus(cam, tlX, tlY, brX, brY);
            });
    AddStep([this] { return new StepGameDisableCamera(mScreen->mCamController); });
    // ENEMY KILL
    AddStep([] { return new StepGameTowerIntro; });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([this]
            {
                GameObject * enemy = GetObjectInCell(cellEnemy);
                return new StepGameWaitEnemyKilled(enemy, mScreen->mGameMap);
            });
    AddStep([] { return new StepDelay(1.0f); });
    AddStep([this] { return new StepGameEnemyKilled(); });
    AddStep([this] { return new StepGameQuickUnitButton(mScreen->mHUD, indUnit2); });
    // ===== PART 4 =====
    // TODO remove next 2 steps when trying full tutorial
    // AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    // AddStep([this] { return new StepGameWaitTurn(mScreen); });
    // BUILD WALL FROM TOWER
    AddStep([] { return new StepGameWallBuildIntro(); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelActions] { return new StepGameWallBuildIcon(panelActions); });
    AddStep([this]
            {
                const Cell2D & cellStart = mScreen->mOverlayWall->GetCellStart();
                const Cell2D target(37, 27);
                return new StepGameWallBuildStart(mScreen->mIsoMap, cellStart, target);
            });
    AddStep([this, local]
            {
                const auto unit = local->GetUnit(indUnit2);
                const Cell2D cellEnd(37, 30);
                return new StepGameWallBuildEnd(mScreen->mIsoMap, unit, cellEnd);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // UPGRADE UNIT
    AddStep([panelActions] { return new StepGameUpgradeIntro(panelActions, "TUT_GAME_UPGRADE_1b"); });
    AddStep([this] { return new StepGameUpgradeUnit(mScreen->mHUD, false); });
    AddStep([this] { return new StepGameDisableCamera(mScreen->mCamController); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([this] { return new StepGameWaitTurn(mScreen); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER SECOND ENERGY GENERATOR
    //AddStep([] { return new StepGameMoveCamera(500, 200); });
    AddStep([this, local]
            {
                const GameObject * gen = GetObjectInCell(cellEneGen2);

                return new StepGameConquerEnergyGenIntro(gen);
            });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::MOVE); });
    AddStep([this, local]
            {
                const auto unit = local->GetUnit(indUnit2);
                const GameObject * gen = GetObjectInCell(cellEneGen2);
                const sgl::core::Pointd2D p0(650, 250);

                return new StepGameConquerStructSimple(unit, gen, mScreen->mIsoMap, p0);
            });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([this] { return new StepGameWaitTurn(mScreen); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE VIEW BACK TO BASE
    AddStep([panelTurn] { return new StepGameBackToBase(panelTurn, "TUT_GAME_BACK_TO_BASE_1b"); });
    AddStep([] { return new StepDelay(1.0f); });
    // ===== PART 5 =====
    // INTRODUCE MISSION GOALS
    AddStep([localBase] { return new StepGameSelectBase(localBase); });
    AddStep([panelActions] { return new StepGameMissionGoalsIcon(panelActions, true); });
    AddStep([this] { return new StepGameMissionGoalsDialog(mScreen->mHUD); });
    AddStep([this] { return new StepGameDisableCamera(mScreen->mCamController); });
    AddStep([] { return new StepDelay(0.5f); });
    // BACK TO UNIT 2
    AddStep([this] { return new StepGameQuickUnitButton(mScreen->mHUD, indUnit2); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT SECOND ENERGY GENERATOR
    AddStep([]
            {
                const sgl::core::Pointd2D p0(1250, 350);
                return new StepGameConnectStructIntro(p0);
            });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this]
            {
                const sgl::core::Pointd2D p0(1100, 350);
                const Cell2D & cellStart = mScreen->mOverlayCellConquest->GetCellStart();
                const Cell2D target(30, 26);
                return new StepGameConquerCellsSimple(mScreen->mIsoMap, cellStart, target, p0);
            });
    AddStep([this, local]
            {
                const auto unit = local->GetUnit(indUnit2);
                const sgl::core::Pointd2D p0(1000, 550);
                const Cell2D cellEnd(36, 26);
                return new StepGameConquerCellsEnd(mScreen->mIsoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE VIEW BACK TO BASE
    AddStep([panelTurn] { return new StepGameBackToBase(panelTurn, "TUT_GAME_BACK_TO_BASE_1c"); });
    AddStep([] { return new StepDelay(1.0f); });
    // COLLECT PRIMARY MISSION GOAL
    AddStep([localBase] { return new StepGameSelectBase(localBase); });
    AddStep([panelActions] { return new StepGameMissionGoalsIcon(panelActions, false); });
    AddStep([this] { return new StepGamePrimaryMissionGoal(mScreen->mHUD); });
}

TutorialGameIntro::~TutorialGameIntro()
{
    // re-enable camera in game in case tutorial is quit
    mScreen->mCamController->SetEnabled(true);
}

GameObject * TutorialGameIntro::GetObjectInCell(const Cell2D & cell) const
{
    const GameMapCell gmc = mScreen->mGameMap->GetCell(cell.row, cell.col);
    return gmc.objTop;
}

GameObject *TutorialGameIntro::GetObjectInCell(int r, int c) const
{
    const GameMapCell gmc = mScreen->mGameMap->GetCell(r, c);
    return gmc.objTop;
}

} // namespace game
