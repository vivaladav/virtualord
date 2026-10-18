#include "Tutorial/TutorialGame2.h"

#include "CameraMapController.h"
#include "Game.h"
#include "GameMap.h"
#include "Player.h"
#include "GameObjects/Barracks.h"
#include "GameObjects/Base.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "GameObjects/Unit.h"
#include "Indicators/OverlayCellConquest.h"
#include "Indicators/OverlayWall.h"
#include "Screens/ScreenGame.h"
#include "Tutorial/StepAISetActive.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepDummy.h"
#include "Tutorial/StepGameAddEnemy.h"
#include "Tutorial/StepGameIntro2.h"
#include "Tutorial/StepGameBackToBase.h"
#include "Tutorial/StepGameBuildStructAreaFree.h"
#include "Tutorial/StepGameBuildStructure.h"
#include "Tutorial/StepGameBuildUnitEnd.h"
#include "Tutorial/StepGameBuildWallLine.h"
#include "Tutorial/StepGameClickActionButton.h"
#include "Tutorial/StepGameCollectObjects.h"
#include "Tutorial/StepGameConnectStructFree.h"
#include "Tutorial/StepGameConnectWall.h"
#include "Tutorial/StepGameConquerStructFree.h"
#include "Tutorial/StepGameDialogTradingBuy.h"
#include "Tutorial/StepGameInfoArea.h"
#include "Tutorial/StepGameMissionGoalsIcon.h"
#include "Tutorial/StepGameMoveCameraOverCell.h"
#include "Tutorial/StepGameMoveCameraOverObject.h"
#include "Tutorial/StepGameMoveUnitFree.h"
#include "Tutorial/StepGamePanelHit.h"
#include "Tutorial/StepGamePrimaryMissionGoal.h"
#include "Tutorial/StepGameResourcesBar.h"
#include "Tutorial/StepGameSecondaryMissionGoal.h"
#include "Tutorial/StepGameSelectObject.h"
#include "Tutorial/StepGameSetCollectableGeneratorTurns.h"
#include "Tutorial/StepGameSetObjectEnergy.h"
#include "Tutorial/StepGameSetObjectFatalHit.h"
#include "Tutorial/StepGameSetObjectHealth.h"
#include "Tutorial/StepGameSetObjectIndestructible.h"
#include "Tutorial/StepGameSetupResearch.h"
#include "Tutorial/StepGameSetupResearchIcon.h"
#include "Tutorial/StepGameSingleInfo.h"
#include "Tutorial/StepGameTechTreeDialog.h"
#include "Tutorial/StepGameTechTreeIcon.h"
#include "Tutorial/StepGameTradeIcon.h"
#include "Tutorial/StepGameUnit.h"
#include "Tutorial/StepGameUnitAttackBurst.h"
#include "Tutorial/StepGameUnitAttackIcon.h"
#include "Tutorial/StepGameUnitAttackFree.h"
#include "Tutorial/StepGameWaitEnemiesKilled.h"
#include "Tutorial/StepGameWaitObjectDestroyed.h"
#include "Tutorial/StepGameWaitStructCanBuild.h"
#include "Tutorial/StepGameWaitUnitCanBuild.h"
#include "Tutorial/StepGameWallBuildIntro.h"
#include "Tutorial/StepSaveGame.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/GameHUD.h"
#include "Widgets/PanelObjectActions.h"

#include <vector>

namespace
{
using namespace game;

constexpr unsigned int indWorker1 = 0;
constexpr unsigned int indSoldier1 = 1;
constexpr unsigned int indWorker2 = 2;
constexpr unsigned int indSoldier2 = 3;

constexpr int turnsCollGenMin = 2;
constexpr int turnsCollGenMax = 2;
constexpr int turnsCollGenMin2 = 5;
constexpr int turnsCollGenMax2 = 15;
constexpr int collectablesMin = 2;
constexpr int collectablesMax = 4;

constexpr float energyEnemyDef = 60.f;

constexpr float delayAnimLong = 2.f;

constexpr float speedCamera = 700.f;

const Cell2D cellEneGen1(6, 15);
const Cell2D cellEneGen2(23, 17);
const Cell2D cellEneGen3(5, 26);
const Cell2D cellMatGen1(15, 7);
const Cell2D cellMatGen2(24, 3);
const Cell2D cellMatGen3(15, 26);
const Cell2D cellTarget1(15, 13);
const Cell2D cellEnemy1(29, 22);
const Cell2D cellEnemy2(28, 24);
const Cell2D cellEnemy3(28, 28);
const Cell2D cellEnemy4(27, 30);
const Cell2D cellEnemy5(22, 32);

const Cell2D areaBlobsTL(22, 12);
const Cell2D areaBlobsBR(25, 13);
const Cell2D areaBlobs2TL(2, 46);
const Cell2D areaBlobs2BR(4, 48);
const Cell2D areaDiamondsTL(23, 6);
const Cell2D areaDiamondsBR(24, 9);
const Cell2D areaDiamonds2TL(7,41);
const Cell2D areaDiamonds2BR(9, 42);

const Cell2D areaTreesTL(13, 13);
const Cell2D areaTreesBR(16, 16);
const Cell2D areaTradingPostTL(16, 17);
const Cell2D areaTradingPostBR(20, 21);

const Cell2D targetSoldier1(15, 11);

const Cell2D wallRightTL(12, 29);
const Cell2D wallRightBR(22, 29);
const Cell2D wallCenterTL(24, 23);
const Cell2D wallCenterBR(24, 28);
const Cell2D wallLeftTL(27, 12);
const Cell2D wallLeftBR(27, 21);

constexpr TechUpgradeId upgrade1 = TECH_UP_BASE_IMPROVE_1;

}

namespace game
{

TutorialGame2::TutorialGame2(Screen * screen)
    : TutorialGame(screen, TUTORIAL_MISSION_2)
{
    using namespace sgl;

    auto gs = GetScreen();
    auto game = gs->GetGame();
    auto hud = gs->GetHUD();
    auto gameMap = GetGameMap();
    auto isoMap = GetIsoMap();

    const Player * local = game->GetActivePlayerByIndex(0);
    Player * playerAI = game->GetActivePlayerByIndex(1);

    auto localBase = local->GetBase();
    auto panelActions = hud->GetPanelObjectActions();
    auto panelTurn = hud->GetPanelTurnControl();

    // keep camera enabled in this tutorial
    GetCameraMapController()->SetEnabled(true);

    // ===== SETUP =====
    // set all generators of diamonds and blobs to create them within 2 turns
    AddStep([gameMap]
        {
            return new StepGameSetCollectableGeneratorTurns(areaDiamondsTL, areaDiamondsBR, gameMap,
                                                            ObjectData::TYPE_DIAMONDS,
                                                            turnsCollGenMin, turnsCollGenMax);
        });
    AddStep([gameMap]
        {
            return new StepGameSetCollectableGeneratorTurns(areaDiamonds2TL, areaDiamonds2BR, gameMap,
                                                            ObjectData::TYPE_DIAMONDS,
                                                            turnsCollGenMin, turnsCollGenMax);
        });
    AddStep([gameMap]
        {
            return new StepGameSetCollectableGeneratorTurns(areaBlobsTL, areaBlobsBR, gameMap,
                                                            ObjectData::TYPE_BLOBS,
                                                            turnsCollGenMin, turnsCollGenMax);
        });
    AddStep([gameMap]
        {
            return new StepGameSetCollectableGeneratorTurns(areaBlobs2TL, areaBlobs2BR, gameMap,
                                                            ObjectData::TYPE_BLOBS,
                                                            turnsCollGenMin, turnsCollGenMax);
        });
    // MAKE TREES INDESTRUCTIBLE
    AddStep([gameMap]
    {
        std::vector<GameObject *> trees;
        gameMap->GetObjectsInAreaByType(trees, areaTreesTL, areaTreesBR, ObjectData::TYPE_TREES3);
        return new StepGameSetObjectIndestructible(std::move(trees), true);
    });
    // pause before start
    AddStep([] { return new StepDelay(0.5f); });

    // ===== PART 1 =====
    // INTRO
    AddStep([] { return new StepGameIntro2; });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WORKER 1
    AddStep([local, game]
    {
        const core::Pointd2D p0(1300, 100);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_UNIT_WORKER1);

        auto step = new StepGameWaitStructCanBuild(local, data, ObjectData::TYPE_BASE,
                                                   "TUT_GAME_BUILD_WORKER");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameClickActionButton(game, gs, panelActions, "TUT_GAME_BUILD_WORKER",
                                                  PanelObjectActions::BTN_BUILD_UNIT_BASE,
                                                  delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([hud]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameBuildUnitEnd(hud, "TUT_GAME_BUILD_WORKER", delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([localBase] { return new StepDelay(localBase->GetTimeBuildUnit()); });
    // CONQUER ENERGY GENERATOR 1
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellEneGen1);
        const core::Pointd2D p0(100, 450);
        return new StepGameConquerStructFree(game, gen, isoMap, "TUT_GAME_CONQUER_ENE_GEN", p0);
    });
    // CONNECT ENERGY GENERATOR 1
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellEneGen1);
        const core::Pointd2D p0(1200, 400);
        return new StepGameConnectStructFree(game, gen, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER MATERIAL GENERATOR 1
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellMatGen1);
        const core::Pointd2D p0(1200, 400);
        return new StepGameConquerStructFree(game, gen, isoMap, "TUT_GAME_CONQUER_MAT_GEN", p0);
    });
    // CONNECT MATERIAL GENERATOR 1
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellMatGen1);
        const core::Pointd2D p0(1300, 400);
        return new StepGameConnectStructFree(game, gen, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD BARRACKS
    AddStep([local, game]
    {
        const core::Pointd2D p0(1300, 100);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_BARRACKS);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_BARRACKS_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameClickActionButton(game, gs, panelActions, "TUT_GAME_BUILD_BARRACKS_1",
                                                  PanelObjectActions::BTN_BUILD_STRUCT, delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([hud]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameBuildStructure(hud, nullptr, nullptr, "TUT_GAME_BASE_BUILD_UNIT_3",
                                               TutorialConstants::catGeneric,
                                               TutorialConstants::structBarracks);
        step->SetDelayAnimation(2.f);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const Cell2D areaTL(7, 8);
        const Cell2D areaBR(11, 14);
        const int numToBuild = 1;

        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, areaTL, areaBR,
                                                    ObjectData::TYPE_BARRACKS, numToBuild,
                                                    "TUT_GAME_BUILD_BARRACKS_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    // CONNECT BARRACKS IF NEEDED
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const auto all = local->GetStructuresByType(ObjectData::TYPE_BARRACKS);
        const GameObject * barracks = all.back();
        const core::Pointd2D p0(1200, 400);
        return new StepGameConnectStructFree(game, barracks, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD SOLDIER
    AddStep([local, game]
    {
        const core::Pointd2D p0(1300, 100);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_UNIT_SOLDIER1);

        auto step = new StepGameWaitStructCanBuild(local, data, ObjectData::TYPE_BARRACKS,
                                                   "TUT_GAME_BUILD_LSOLDIER");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameClickActionButton(game, gs, panelActions, "TUT_GAME_BUILD_LSOLDIER",
                                                  PanelObjectActions::BTN_BUILD_UNIT_BARRACKS,
                                                  delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([hud]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameBuildUnitEnd(hud, "TUT_GAME_BUILD_LSOLDIER", delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local]
    {
        const auto all = local->GetStructuresByType(ObjectData::TYPE_BARRACKS);
        const auto barracks = static_cast<Barracks *>(all.back());
        return new StepDelay(barracks->GetTimeBuildUnit());
    });
    // MOVE SOLDIER NEAR TREES
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game, isoMap, local]
    {
        const core::Pointd2D p0(1300, 500);
        const auto all = local->GetUnitsByType(ObjectData::TYPE_UNIT_SOLDIER1);
        const auto unit = all.back();
        return new StepGameMoveUnitFree(game, unit, isoMap, "TUT_GAME_MOVE_SOLDIER",
                                        targetSoldier1, p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // MAKE TREES DESTRUCTIBLE AGAIN
    AddStep([gameMap]
    {
        std::vector<GameObject *> trees;
        gameMap->GetObjectsInAreaByType(trees, areaTreesTL, areaTreesBR, ObjectData::TYPE_TREES3);
        return new StepGameSetObjectIndestructible(std::move(trees), false);
    });
    // EXPLAIN ATTACK
    AddStep([]
        {
            const core::Pointd2D p0(300, 500);
            return new StepGameSingleInfo(p0, "TUT_GAME_ATTACK_1");
        });
    AddStep([game, gs, panelActions]
        {
            const core::Pointd2D p0(450, 650);
            return new StepGameUnitAttackIcon(game, gs, panelActions, p0);
        });
    AddStep([hud]
        {
            const core::Pointd2D p0(150, 600);

            auto panelShot = hud->GetPanelShotType();
            return new StepGameUnitAttackBurst(panelShot, "TUT_GAME_ATTACK_3", p0);
        });
    AddStep([isoMap]
    {
        const core::Pointd2D p0(200, 600);
        return new StepGamePanelHit(isoMap, areaTreesBR, areaTreesTL, p0);
    });
    // LOWER TREE'S HEALTH
    AddStep([this]
    {
        const float health = 20.f;
        GameObject * enemy = GetObjectInCell(cellTarget1);
        return new StepGameSetObjectHealth(enemy, health);
    });
    // ATTACK TREE
    AddStep([this, isoMap, game, gameMap]
    {
        const auto unit = static_cast<Unit *>(gameMap->GetObject(targetSoldier1.row,
                                                                 targetSoldier1.col));
        const GameObject * enemy = GetObjectInCell(cellTarget1);
        const core::Pointd2D p0(250, 500);

        auto step = new StepGameUnitAttackFree(game, unit, enemy, isoMap,
                                               gameMap, "TUT_GAME_ATTACK_6");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    AddStep([this, isoMap, game, gameMap]
    {
        const GameObject * enemy = GetObjectInCell(cellTarget1);
        const core::Pointd2D p0(250, 500);

        auto step = new StepGameWaitObjectDestroyed(game, enemy, isoMap, gameMap,
                                                    "TUT_GAME_DESTROY_TREE");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // PRESENT DIAMONDS
    AddStep([this, local, isoMap]
    {
        const core::Pointd2D p0(600, 200);
        return new StepGameInfoArea(isoMap, areaDiamondsTL, areaDiamondsBR,
                                    "TUT_GAME_MOVE_UNIT_4a", "TUT_GAME_MOVE_UNIT_4b", p0);
    });
    // COLLECT DIAMONDS
    AddStep([isoMap, gameMap, local, game]
    {
        const core::Pointd2D p0(1300, 400);
        return new StepGameCollectObjects(ObjectData::TYPE_DIAMONDS, gameMap, isoMap,
                                          areaDiamondsTL, areaDiamondsBR,
                                          "TUT_GAME_COLLECT_DIAMONDS", p0);
    });
    // MOVE CAMERA OVER BLOBS
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([isoMap]
    {
        const Cell2D cell(20, 11);
        return new StepGameMoveCameraOverCell(cell, isoMap, speedCamera);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // PRESENT BLOBS
    AddStep([this, local, isoMap]
    {
        const core::Pointd2D p0(1200, 600);
        return new StepGameInfoArea(isoMap, areaBlobsTL, areaBlobsBR,
                                    "TUT_GAME_MOVE_UNIT_5a", "TUT_GAME_MOVE_UNIT_5b", p0);
    });
    // COLLECT BLOBS
    AddStep([isoMap, gameMap, local, game]
    {
        const core::Pointd2D p0(1300, 600);
        return new StepGameCollectObjects(ObjectData::TYPE_BLOBS, gameMap, isoMap,
                                          areaBlobsTL, areaBlobsBR,
                                          "TUT_GAME_COLLECT_BLOBS", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA OVER MATERIAL GENERATOR
    AddStep([this]
    {
        const GameObject * gen = GetObjectInCell(cellMatGen2);
        return new StepGameMoveCameraOverObject(gen, speedCamera);
    });
    // CONQUER MATERIAL GENERATOR 2
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellMatGen2);
        const core::Pointd2D p0(1200, 400);
        return new StepGameConquerStructFree(game, gen, isoMap, "TUT_GAME_CONQUER_MAT_GEN", p0);
    });
    // CONNECT MATERIAL GENERATOR 2
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellMatGen2);
        const core::Pointd2D p0(1300, 400);
        return new StepGameConnectStructFree(game, gen, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA OVER ENERGY GENERATOR
    AddStep([this]
    {
        const GameObject * gen = GetObjectInCell(cellEneGen2);
        return new StepGameMoveCameraOverObject(gen, speedCamera);
    });
    // CONQUER ENERGY GENERATOR 2
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellEneGen2);
        const core::Pointd2D p0(100, 450);
        return new StepGameConquerStructFree(game, gen, isoMap, "TUT_GAME_CONQUER_ENE_GEN", p0);
    });
    // CONNECT ENERGY GENERATOR 2
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellEneGen2);
        const core::Pointd2D p0(1200, 400);
        return new StepGameConnectStructFree(game, gen, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD RESEARCH CENTER
    AddStep([local, game]
    {
        const core::Pointd2D p0(1300, 100);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_RESEARCH_CENTER);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_RES_CEN_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameClickActionButton(game, gs, panelActions, "TUT_GAME_BUILD_RES_CEN_1",
                                                  PanelObjectActions::BTN_BUILD_STRUCT, delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([hud]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameBuildStructure(hud, "TUT_GAME_BUILD_RES_CEN_2", nullptr,
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               TutorialConstants::catTech,
                                               TutorialConstants::structResearchCenter);
        step->SetDelayAnimation(2.f);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const Cell2D areaTL(7, 8);
        const Cell2D areaBR(12, 16);
        const int numToBuild = 1;

        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, areaTL, areaBR,
                                                    ObjectData::TYPE_RESEARCH_CENTER, numToBuild,
                                                    "TUT_GAME_BUILD_RES_CEN_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    // CONNECT RESEARCH CENTER IF NEEDED
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const auto all = local->GetStructuresByType(ObjectData::TYPE_RESEARCH_CENTER);
        const GameObject * rc = all.back();
        const core::Pointd2D p0(1300, 400);
        return new StepGameConnectStructFree(game, rc, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT RESEARCH CENTER
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const auto all = local->GetStructuresByType(ObjectData::TYPE_RESEARCH_CENTER);
        const GameObject * rc = all.back();

        return new StepGameSelectObject(game, isoMap, rc, "TUT_GAME_RES_CEN_1", p0,
                                        delayAnimLong);
    });
    // SETUP RESEARCH
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1000, 650);
        return new StepGameSetupResearchIcon(game, gs, panelActions, p0);
    });
    AddStep([hud]
    {
        return new StepGameSetupResearch(hud, { 100, 90, 80 });
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // EXPLAIN RESOURCES BAR
    AddStep([hud] { return new StepGameResourcesBar(hud); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE SOLDIER UNIT TO RIGHT
    AddStep([]
    {
        const core::Pointd2D p0(1300, 200);
        return new StepGameSingleInfo(p0, "TUT_GAME_MOVE_UNIT_3");
    });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([] { return new StepGameMoveCamera(0.f, 250.f); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game, isoMap, local]
    {
        const core::Pointd2D p0(1300, 200);
        const Cell2D target(14,21);
        const auto all = local->GetUnitsByType(ObjectData::TYPE_UNIT_SOLDIER1);
        const auto unit = all.back();
        return new StepGameMoveUnitFree(game, unit, isoMap, "TUT_GAME_MOVE_SOLDIER",
                                        target, p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
     // CONQUER MATERIAL GENERATOR 3
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellMatGen3);
        const core::Pointd2D p0(1200, 400);
        return new StepGameConquerStructFree(game, gen, isoMap, "TUT_GAME_CONQUER_MAT_GEN", p0);
    });
    // CONNECT MATERIAL GENERATOR 3
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellMatGen3);
        const core::Pointd2D p0(1300, 400);
        return new StepGameConnectStructFree(game, gen, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER ENERGY GENERATOR 3
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellEneGen3);
        const core::Pointd2D p0(100, 450);
        return new StepGameConquerStructFree(game, gen, isoMap, "TUT_GAME_CONQUER_ENE_GEN", p0);
    });
    // MOVE VIEW BACK TO BASE
    AddStep([panelTurn]
    {
        const core::Pointd2D p0(800, 500);
        return new StepGameBackToBase(panelTurn, "TUT_GAME_BACK_TO_BASE_1b", p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // COLLECT SECONDARY MISSION GOAL
    AddStep([localBase, game, isoMap]
    {
        const core::Pointd2D p0(500, 200);
        return new StepGameSelectObject(game, isoMap, localBase, "TUT_GAME_BASE_4", p0);
    });
    AddStep([game, gs, panelActions] { return new StepGameMissionGoalsIcon(game, gs, panelActions, false); });
    AddStep([hud]
    {
        const int goal = 0;
        return new StepGameSecondaryMissionGoal(hud, goal);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA
    AddStep([this]
    {
        const GameObject * gen = GetObjectInCell(cellEneGen3);
        return new StepGameMoveCameraOverObject(gen, speedCamera);
    });
    // CONNECT ENERGY GENERATOR 3
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellEneGen3);
        const core::Pointd2D p0(1200, 300);
        return new StepGameConnectStructFree(game, gen, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA
    AddStep([isoMap]
    {
        const Cell2D target(15, 29);
        return new StepGameMoveCameraOverCell(target, isoMap, speedCamera);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL RIGHT
    AddStep([]
    {
        const core::Pointd2D p0(1300, 450);
        return new StepGameWallBuildIntro(p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 450);
        auto step = new StepGameBuildWallLine(gameMap, isoMap, wallRightTL, wallRightBR,
                                              "TUT_GAME_BUILD_WALL_5");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT RESEARCH CENTER AND UNLOCK UPGRADE
    AddStep([this, local]
    {
        TutorialStep * step = nullptr;

        if(local->IsUpgradeUnlocked(upgrade1))
            step = new StepDummy;
        else
        {
            const auto all = local->GetStructuresByType(ObjectData::TYPE_RESEARCH_CENTER);
            const GameObject * rc = all.back();
            const float speed = 1000.f;
            step = new StepGameMoveCameraOverObject(rc, speed);

            mUpgrade1Unlocked = true;
        }

        return step;
    });
    AddStep([local]
    {
        TutorialStep * step = nullptr;

        if(local->IsUpgradeUnlocked(upgrade1))
            step = new StepDummy;
        else
            step = new StepDelay(0.5f);

        return step;
    });
    AddStep([local]
    {
        TutorialStep * step = nullptr;

        if(local->IsUpgradeUnlocked(upgrade1))
            step = new StepDummy;
        else
        {
            const core::Pointd2D p0(685, 200);
            step = new StepGameSingleInfo(p0, "TUT_GAME_RES_CEN_2");
        }

        return step;
    });
    AddStep([this, game, isoMap, local]
    {
        TutorialStep * step = nullptr;

        if(local->IsUpgradeUnlocked(upgrade1))
            step = new StepDummy;
        else
        {
            const core::Pointd2D p0(710, 250);
            const auto all = local->GetStructuresByType(ObjectData::TYPE_RESEARCH_CENTER);
            const GameObject * rc = all.back();
            step = new StepGameSelectObject(game, isoMap, rc, "TUT_GAME_RES_CEN_1", p0);
        }

        return step;
    });
    AddStep([game, gs, panelActions, local]
    {
        TutorialStep * step = nullptr;

        if(local->IsUpgradeUnlocked(upgrade1))
            step = new StepDummy;
        else
        {
            const core::Pointd2D p0(1000, 650);
            step = new StepGameTechTreeIcon(game, gs, panelActions, p0);
        }

        return step;
    });
    AddStep([local]
    {
        TutorialStep * step = nullptr;

        if(local->IsUpgradeUnlocked(upgrade1))
            step = new StepDummy;
        else
            step = new StepDelay(0.5f);

        return step;
    });
    AddStep([hud, local]
    {
        TutorialStep * step = nullptr;

        if(local->IsUpgradeUnlocked(upgrade1))
            step = new StepDummy;
        else
            step = new StepGameTechTreeDialog(hud, upgrade1, true);

        return step;
    });
    AddStep([local]
    {
        TutorialStep * step = nullptr;

        if(local->IsUpgradeUnlocked(upgrade1))
            step = new StepDummy;
        else
            step = new StepDelay(0.5f);

        return step;
    });
    // MOVE CAMERA
    AddStep([this, isoMap, local]
    {
        TutorialStep * step = nullptr;

        if(!mUpgrade1Unlocked)
            step = new StepDummy;
        else
        {
            const Cell2D target((wallRightTL.row + wallRightBR.row) / 2, wallRightTL.col);
            step = new StepGameMoveCameraOverCell(target, isoMap, speedCamera);
        }

        return step;
    });
    AddStep([this, local]
    {
        TutorialStep * step = nullptr;

        if(mUpgrade1Unlocked)
            step = new StepDummy;
        else
            step = new StepDelay(0.5f);

        return step;
    });
    // CONNECT WALL RIGHT
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 350);
        auto step = new StepGameConnectWall(gameMap, isoMap, wallRightTL, wallRightBR,
                                            "TUT_GAME_CONNECT_WALL");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    // BUILD GATE IN RIGHT WALL
    AddStep([local, game]
    {
        const core::Pointd2D p0(1300, 100);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_WALL_GATE);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_GATE_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameClickActionButton(game, gs, panelActions, "TUT_GAME_BUILD_GATE_1",
                                                  PanelObjectActions::BTN_BUILD_STRUCT, delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([hud]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameBuildStructure(hud,"TUT_GAME_BUILD_DTOWER_3",
                                               "TUT_GAME_BUILD_GATE_2",
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               TutorialConstants::catDefenses,
                                               TutorialConstants::structGate);
        step->SetDelayAnimation(2.f);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const unsigned int numToBuild = 1;
        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, wallRightTL, wallRightBR,
                                                    ObjectData::TYPE_WALL_GATE, numToBuild,
                                                    "TUT_GAME_BUILD_GATE_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD TOWERS IN RIGHT WALL
    AddStep([local, game]
    {
        const core::Pointd2D p0(1300, 600);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_DEFENSIVE_TOWER);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_DTOWERS");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameClickActionButton(game, gs, panelActions, "TUT_GAME_BUILD_DTOWERS",
                                                  PanelObjectActions::BTN_BUILD_STRUCT, delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([hud]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameBuildStructure(hud,"TUT_GAME_BUILD_DTOWER_3",
                                               "TUT_GAME_BUILD_DTOWER_4",
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               TutorialConstants::catDefenses,
                                               TutorialConstants::structDefTower);
        step->SetDelayAnimation(2.f);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const unsigned int numToBuild = 2;
        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, wallRightTL, wallRightBR,
                                                    ObjectData::TYPE_DEFENSIVE_TOWER, numToBuild,
                                                    "TUT_GAME_BUILD_DTOWERS");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA
    AddStep([isoMap]
    {
        return new StepGameMoveCameraOverCell(wallCenterBR, isoMap, speedCamera);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL CENTRAL
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 450);
        auto step = new StepGameBuildWallLine(gameMap, isoMap, wallCenterTL, wallCenterBR,
                                              "TUT_GAME_BUILD_WALL_5");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT WALL CENTER
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 150);
        auto step = new StepGameConnectWall(gameMap, isoMap, wallCenterTL, wallCenterBR,
                                            "TUT_GAME_CONNECT_WALL");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    // BUILD TOWERS IN CENTRAL WALL
    AddStep([local, game]
    {
        const core::Pointd2D p0(1300, 600);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_DEFENSIVE_TOWER);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_DTOWERS");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameClickActionButton(game, gs, panelActions, "TUT_GAME_BUILD_DTOWERS",
                                                  PanelObjectActions::BTN_BUILD_STRUCT, delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([hud]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameBuildStructure(hud,"TUT_GAME_BUILD_DTOWER_3",
                                               "TUT_GAME_BUILD_DTOWER_4",
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               TutorialConstants::catDefenses,
                                               TutorialConstants::structDefTower);
        step->SetDelayAnimation(2.f);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const unsigned int numToBuild = 2;
        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, wallCenterTL, wallCenterBR,
                                                    ObjectData::TYPE_DEFENSIVE_TOWER, numToBuild,
                                                    "TUT_GAME_BUILD_DTOWERS");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA
    AddStep([isoMap]
    {
        return new StepGameMoveCameraOverCell(wallLeftBR, isoMap, speedCamera);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL LEFT
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(100, 500);
        auto step = new StepGameBuildWallLine(gameMap, isoMap, wallLeftTL, wallLeftBR,
                                              "TUT_GAME_BUILD_WALL_5");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT WALL LEFT
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 350);
        auto step = new StepGameConnectWall(gameMap, isoMap, wallLeftTL, wallLeftBR,
                                            "TUT_GAME_CONNECT_WALL");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    // BUILD GATE IN LEFT WALL
    AddStep([local, game]
    {
        const core::Pointd2D p0(1300, 100);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_WALL_GATE);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_GATE_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameClickActionButton(game, gs, panelActions, "TUT_GAME_BUILD_GATE_1",
                                                  PanelObjectActions::BTN_BUILD_STRUCT, delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([hud]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameBuildStructure(hud,"TUT_GAME_BUILD_DTOWER_3",
                                               "TUT_GAME_BUILD_GATE_2",
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               TutorialConstants::catDefenses,
                                               TutorialConstants::structGate);
        step->SetDelayAnimation(2.f);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const unsigned int numToBuild = 1;
        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, wallLeftTL, wallLeftBR,
                                                    ObjectData::TYPE_WALL_GATE, numToBuild,
                                                    "TUT_GAME_BUILD_GATE_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD TOWERS IN LEFT WALL
    AddStep([local, game]
    {
        const core::Pointd2D p0(1300, 600);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_DEFENSIVE_TOWER);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_DTOWERS");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameClickActionButton(game, gs, panelActions, "TUT_GAME_BUILD_DTOWERS",
                                                  PanelObjectActions::BTN_BUILD_STRUCT, delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([hud]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameBuildStructure(hud,"TUT_GAME_BUILD_DTOWER_3",
                                               "TUT_GAME_BUILD_DTOWER_4",
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               TutorialConstants::catDefenses,
                                               TutorialConstants::structDefTower);
        step->SetDelayAnimation(2.f);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const unsigned int numToBuild = 2;
        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, wallLeftTL, wallLeftBR,
                                                    ObjectData::TYPE_DEFENSIVE_TOWER, numToBuild,
                                                    "TUT_GAME_BUILD_DTOWERS");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA
    AddStep([this, local]
    {
        const auto all = local->GetStructuresByType(ObjectData::TYPE_RESEARCH_CENTER);
        const GameObject * rc = all.back();
        const float speed = 1000.f;
        return new StepGameMoveCameraOverObject(rc, speed);
    });
    // SELECT RESEARCH CENTER AND UNLOCK UPGRADE
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([]
    {
        const core::Pointd2D p0(685, 200);
        return new StepGameSingleInfo(p0, "TUT_GAME_RES_CEN_2");
    });
    AddStep([this, game, isoMap, local]
    {
        const core::Pointd2D p0(710, 250);
        const auto all = local->GetStructuresByType(ObjectData::TYPE_RESEARCH_CENTER);
        const GameObject * rc = all.back();
        return new StepGameSelectObject(game, isoMap, rc, "TUT_GAME_RES_CEN_1", p0);
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1000, 650);
        return new StepGameTechTreeIcon(game, gs, panelActions, p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([hud] { return new StepGameTechTreeDialog(hud, TECH_UP_TRADING_POST, false); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA
    AddStep([isoMap]
    {
        const Cell2D target(areaTradingPostBR.row, areaTradingPostTL.col);
        return new StepGameMoveCameraOverCell(target, isoMap, speedCamera);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD TRADING POST
    AddStep([local, game]
    {
        const core::Pointd2D p0(1300, 100);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_TRADING_POST);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_TRAD_POST_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameClickActionButton(game, gs, panelActions, "TUT_GAME_BUILD_TRAD_POST_1",
                                                  PanelObjectActions::BTN_BUILD_STRUCT, delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([hud]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameBuildStructure(hud, nullptr, "TUT_GAME_BUILD_TRAD_POST_2",
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               TutorialConstants::catGeneric,
                                               TutorialConstants::structTradingPost);
        step->SetDelayAnimation(2.f);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const unsigned int numToBuild = 1;
        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, areaTradingPostTL,
                                                    areaTradingPostBR, ObjectData::TYPE_TRADING_POST,
                                                    numToBuild, "TUT_GAME_BUILD_TRAD_POST_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    // CONNECT TRADING POST IF NEEDED
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const auto all = local->GetStructuresByType(ObjectData::TYPE_TRADING_POST);
        const GameObject * rc = all.back();
        const core::Pointd2D p0(1300, 400);
        return new StepGameConnectStructFree(game, rc, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SELECT TRADING POST
    AddStep([game, isoMap, local]
    {
        const core::Pointd2D p0(1300, 200);
        const auto all = local->GetStructuresByType(ObjectData::TYPE_TRADING_POST);
        const GameObject * tp = all.back();
        return new StepGameSelectObject(game, isoMap, tp, "TUT_GAME_BUILD_TRAD_POST_2", p0);
    });
    // OPEN AND USE DIALOG TRADING
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(100, 650);
        return new StepGameTradeIcon(game, gs, panelActions, p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game, local, hud]
    {
        return new StepGameDialogTradingBuy(game, local, hud->GetDialogTrading());
    });
    AddStep([] { return new StepDelay(0.5f); });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // ANNOUNCE ENEMIES ARE COMING
    AddStep([]
    {
        const core::Pointd2D p0(1200, 150);
        return new StepGameSingleInfo(p0, "TUT_GAME_BUILD_SOLDIER_1");
    });
    // MOVE CAMERA
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local]
    {
        const auto all = local->GetStructuresByType(ObjectData::TYPE_BARRACKS);
        const GameObject * obj = all.back();
        return new StepGameMoveCameraOverObject(obj, speedCamera);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD SOLDIER
    AddStep([local, game]
    {
        const core::Pointd2D p0(1300, 100);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_UNIT_SOLDIER1);

        auto step = new StepGameWaitStructCanBuild(local, data, ObjectData::TYPE_BARRACKS,
                                                   "TUT_GAME_BUILD_LSOLDIER");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameClickActionButton(game, gs, panelActions, "TUT_GAME_BUILD_LSOLDIER",
                                                  PanelObjectActions::BTN_BUILD_UNIT_BARRACKS,
                                                  delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([hud]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameBuildUnitEnd(hud, "TUT_GAME_BUILD_LSOLDIER", delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local]
    {
        const auto all = local->GetStructuresByType(ObjectData::TYPE_BARRACKS);
        const auto barracks = static_cast<Barracks *>(all.back());
        return new StepDelay(barracks->GetTimeBuildUnit());
    });
    // ADD ENEMIES
    AddStep([this, playerAI]
    {
        return new StepGameAddEnemy(GetGameMap(), playerAI, ObjectData::TYPE_UNIT_SCOUT1,
                                    cellEnemy1, false);
    });
    AddStep([this, playerAI]
    {
        return new StepGameAddEnemy(GetGameMap(), playerAI, ObjectData::TYPE_UNIT_SOLDIER1,
                                    cellEnemy2, false);
    });
    AddStep([this, playerAI]
    {
        return new StepGameAddEnemy(GetGameMap(), playerAI, ObjectData::TYPE_UNIT_SOLDIER1,
                                    cellEnemy3, false);
    });
    AddStep([this, playerAI]
    {
        return new StepGameAddEnemy(GetGameMap(), playerAI, ObjectData::TYPE_UNIT_SOLDIER1,
                                    cellEnemy4, false);
    });
    AddStep([this, playerAI]
    {
        return new StepGameAddEnemy(GetGameMap(), playerAI, ObjectData::TYPE_UNIT_SCOUT1,
                                    cellEnemy5, false);
    });
    // SET ENERGY OF ENEMIES
    AddStep([this]
    {
        std::vector<GameObject *> enemies;
        enemies.emplace_back(GetObjectInCell(cellEnemy2));
        enemies.emplace_back(GetObjectInCell(cellEnemy3));
        enemies.emplace_back(GetObjectInCell(cellEnemy4));
        enemies.emplace_back(GetObjectInCell(cellEnemy5));

        return new StepGameSetObjectEnergy(std::move(enemies), energyEnemyDef);
    });
    // DISABLE FATAL HIT FOR ENEMIES
    AddStep([this]
    {
        std::vector<GameObject *> enemies;
        enemies.emplace_back(GetObjectInCell(cellEnemy2));
        enemies.emplace_back(GetObjectInCell(cellEnemy3));
        enemies.emplace_back(GetObjectInCell(cellEnemy4));
        enemies.emplace_back(GetObjectInCell(cellEnemy5));

        return new StepGameSetObjectFatalHit(std::move(enemies), false);
    });
    // RE-ENABLE AI
    AddStep([playerAI] { return new StepAISetActive(playerAI->GetAI(), true); });
    // SELECT SOLDIER
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 450);
        const auto all = local->GetUnitsByType(ObjectData::TYPE_UNIT_SOLDIER1);
        const auto unit = all.back();
        return new StepGameUnit(game, isoMap, unit, p0);
    });
    // MOVE CAMERA
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game] { return new StepGameMoveCamera(0, 350); });
    // MOVE SOLDIER DOWN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game, isoMap, local]
    {
        const core::Pointd2D p0(1300, 200);
        const Cell2D target(19,22);
        const auto all = local->GetUnitsByType(ObjectData::TYPE_UNIT_SOLDIER1);
        const auto unit = all.back();
        return new StepGameMoveUnitFree(game, unit, isoMap, "TUT_GAME_MOVE_SOLDIER",
                                        target, p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // ANNOUNCE FIGHT
    AddStep([]
    {
        const core::Pointd2D p0(600, 200);
        return new StepGameSingleInfo(p0, "TUT_GAME_KILL_ENEMIES");
    });
    // WAIT FOR PLAYER TO KILL ALL ENEMIES
    AddStep([this]
    {
        std::vector<const GameObject *> enemies;
        enemies.emplace_back(GetObjectInCell(cellEnemy2));
        enemies.emplace_back(GetObjectInCell(cellEnemy3));
        enemies.emplace_back(GetObjectInCell(cellEnemy4));
        enemies.emplace_back(GetObjectInCell(cellEnemy5));

        return new StepGameWaitEnemiesKilled(std::move(enemies), GetGameMap());
    });
    // MOVE VIEW BACK TO BASE
    AddStep([panelTurn]
    {
        const core::Pointd2D p0(800, 300);
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

TutorialGame2::~TutorialGame2()
{
    // make sure trees are no longer indestructible
    std::vector<GameObject *> trees;
    GetGameMap()->GetObjectsInAreaByType(trees, areaTreesTL, areaTreesBR, ObjectData::TYPE_TREES3);

    for(GameObject * obj : trees)
        obj->SetIndestructible(false);
}

} // namespace game
