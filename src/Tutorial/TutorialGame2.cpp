#include "Tutorial/TutorialGame2.h"

#include "CameraMapController.h"
#include "Game.h"
#include "GameMap.h"
#include "Player.h"
#include "GameObjects/Base.h"
#include "GameObjects/Unit.h"
#include "Indicators/OverlayCellConquest.h"
#include "Indicators/OverlayWall.h"
#include "Screens/ScreenGame.h"
#include "Tutorial/StepAISetActive.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepGameAddEnemy.h"
#include "Tutorial/StepGameIntro2.h"
#include "Tutorial/StepGameBackToBase.h"
#include "Tutorial/StepGameBuildStructIntro.h"
#include "Tutorial/StepGameBuildStructure.h"
#include "Tutorial/StepGameBuildTowerEnd.h"
#include "Tutorial/StepGameBuildUnitEnd.h"
#include "Tutorial/StepGameBuildUnitStart.h"
#include "Tutorial/StepGameClearSelection.h"
#include "Tutorial/StepGameCollectObjects.h"
#include "Tutorial/StepGameConnectStructIntro.h"
#include "Tutorial/StepGameConquerCellsEnd.h"
#include "Tutorial/StepGameConquerCellsSimple.h"
#include "Tutorial/StepGameConquerGeneratorIntro.h"
#include "Tutorial/StepGameConquerEnergyGenIntro.h"
#include "Tutorial/StepGameConquerMaterialGenIntro.h"
#include "Tutorial/StepGameConquerStructChoice.h"
#include "Tutorial/StepGameConquerStructSimple.h"
#include "Tutorial/StepGameDisableCamera.h"
#include "Tutorial/StepGameEndTurnSimple.h"
#include "Tutorial/StepGameEnemyIntro.h"
#include "Tutorial/StepGameMakeEnemyAttack.h"
#include "Tutorial/StepGameMissionGoalsIcon.h"
#include "Tutorial/StepGameMoveCameraOverCell.h"
#include "Tutorial/StepGameMoveCameraOverObject.h"
#include "Tutorial/StepGameMoveUnitSimple.h"
#include "Tutorial/StepGameMoveUnitToArea.h"
#include "Tutorial/StepGamePanelHit.h"
#include "Tutorial/StepGameQuickUnitButton.h"
#include "Tutorial/StepGameResourcesBar.h"
#include "Tutorial/StepGameSecondaryMissionGoal.h"
#include "Tutorial/StepGameSelectObject.h"
#include "Tutorial/StepGameSetCollectableGeneratorTurns.h"
#include "Tutorial/StepGameSetCollectableUnits.h"
#include "Tutorial/StepGameSetObjectAttackMode.h"
#include "Tutorial/StepGameSetObjectEnergy.h"
#include "Tutorial/StepGameSetObjectFatalHit.h"
#include "Tutorial/StepGameSetObjectHealth.h"
#include "Tutorial/StepGameSetObjectPerfectShot.h"
#include "Tutorial/StepGameSetSelectionDefaultAction.h"
#include "Tutorial/StepGameSetSelectionActiveAction.h"
#include "Tutorial/StepGameSetupResearch.h"
#include "Tutorial/StepGameSetupResearchIcon.h"
#include "Tutorial/StepGameSingleInfo.h"
#include "Tutorial/StepGameTechTreeDialog.h"
#include "Tutorial/StepGameTechTreeIcon.h"
#include "Tutorial/StepGameUnit.h"
#include "Tutorial/StepGameUnitAttackBurst.h"
#include "Tutorial/StepGameUnitAttackIcon.h"
#include "Tutorial/StepGameUnitAttackContinue.h"
#include "Tutorial/StepGameUnitAttackSimple.h"
#include "Tutorial/StepGameUnitConquerCellsIcon.h"
#include "Tutorial/StepGameUpgradeIntro.h"
#include "Tutorial/StepGameUpgradeUnit.h"
#include "Tutorial/StepGameUpgradeUnitFree.h"
#include "Tutorial/StepGameWaitEnemyKilled.h"
#include "Tutorial/StepGameWallBuildEnd.h"
#include "Tutorial/StepGameWaitTurn.h"
#include "Tutorial/StepGameWallBuildIcon.h"
#include "Tutorial/StepGameWallBuildIntro.h"
#include "Tutorial/StepGameWallBuildStart.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/GameHUD.h"
#include "Widgets/PanelObjectActions.h"

namespace
{
using namespace game;

constexpr unsigned int indWorker1 = 0;
constexpr unsigned int indSoldier1 = 1;
constexpr unsigned int indWorker2 = 2;

constexpr int turnsCollGenMin = 1;
constexpr int turnsCollGenMax = 1;
constexpr int turnsCollGenMin2 = 5;
constexpr int turnsCollGenMax2 = 15;
constexpr int collectablesMin = 2;
constexpr int collectablesMax = 4;

constexpr int catGeneric = 0;
constexpr int catDefenses = 1;
constexpr int catTech = 3;
constexpr int structGate = 2;
constexpr int structDefTower = 1;
constexpr int structTradingPost = 3;

const Cell2D cellEneGen1(6, 15);
const Cell2D cellEneGen2(23, 17);
const Cell2D cellEneGen3(5, 26);
const Cell2D cellEneGen4(13, 43);
const Cell2D cellMatGen1(15, 7);
const Cell2D cellMatGen2(24, 3);
const Cell2D cellMatGen3(15, 26);
const Cell2D cellMatGen4(40, 12);
const Cell2D cellBarracks(8, 9);
const Cell2D cellTarget1(15, 13);
const Cell2D cellResCenter(11, 6);
const Cell2D cellTradingPost(17, 11);
const Cell2D cellEnemy1(4, 34);
const Cell2D cellTower1(6, 33);
const Cell2D cellTower2(17, 31);
const Cell2D cellTower3(28, 17);
const Cell2D cellTower4(21, 29);
const Cell2D cellTower5(27, 21);
const Cell2D cellTower6(11, 32);
const Cell2D cellTower7(24, 23);
const Cell2D cellTower8(23, 27);
const Cell2D cellTower9(28, 9);
const Cell2D cellTower10(28, 0);

const Cell2D areaBlobsTL(22, 12);
const Cell2D areaBlobsBR(25, 13);
const Cell2D areaDiamondsTL(23, 6);
const Cell2D areaDiamondsBR(24, 9);

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

    const Player * local = game->GetPlayerByIndex(0);
    Player * playerAI = game->GetPlayerByIndex(1);

    auto localBase = local->GetBase();
    auto panelActions = hud->GetPanelObjectActions();
    auto panelTurn = hud->GetPanelTurnControl();

    // ===== SETUP =====
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    // make AI idle for now
    AddStep([playerAI] { return new StepAISetActive(playerAI->GetAI(), false); });
    // set all generators of diamonds and blobs to create them within 1 turn
    AddStep([gameMap]
        {
            return new StepGameSetCollectableGeneratorTurns(areaDiamondsTL, areaDiamondsBR, gameMap,
                                                            ObjectData::TYPE_DIAMONDS,
                                                            turnsCollGenMin, turnsCollGenMax);
        });
    AddStep([gameMap]
        {
            return new StepGameSetCollectableGeneratorTurns(areaBlobsTL, areaBlobsBR, gameMap,
                                                            ObjectData::TYPE_BLOBS,
                                                            turnsCollGenMin, turnsCollGenMax);
        });
    // pause before start
    AddStep([] { return new StepDelay(0.5f); });

    // ===== PART 1 =====
    // INTRO
    AddStep([] { return new StepGameIntro2; });
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
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker1);
            const core::Pointd2D p0(1300, 450);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER FIRST GENERATOR
    AddStep([this, local, game, isoMap]
        {
            const GameObject * unit = local->GetSelectedObject();
            const GameObject * gen1 = GetObjectInCell(cellEneGen1);
            const GameObject * gen2 = GetObjectInCell(cellMatGen1);
            return new StepGameConquerStructChoice(game, unit, gen1, gen2, isoMap);
        });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::IDLE); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // SET MIN BLOBS AND DIAMONDS TO COLLECT
    AddStep([gameMap]
            {
                return new StepGameSetCollectableUnits(areaDiamondsTL, areaDiamondsBR, gameMap,
                                                       ObjectData::TYPE_DIAMONDS,
                                                       collectablesMin, collectablesMax);
            });
    AddStep([gameMap]
            {
                return new StepGameSetCollectableUnits(areaBlobsTL, areaBlobsBR, gameMap,
                                                       ObjectData::TYPE_BLOBS,
                                                       collectablesMin, collectablesMax);
            });
    // set all generators of diamonds and blobs to create new object in more than 5 turns
    AddStep([gameMap]
            {
                return new StepGameSetCollectableGeneratorTurns(areaDiamondsTL, areaDiamondsBR,
                                                                gameMap, ObjectData::TYPE_DIAMONDS,
                                                                turnsCollGenMin2, turnsCollGenMax2);
            });
    AddStep([gameMap]
            {
                return new StepGameSetCollectableGeneratorTurns(areaBlobsTL, areaBlobsBR, gameMap,
                                                                ObjectData::TYPE_BLOBS,
                                                                turnsCollGenMin2, turnsCollGenMax2);
            });
    // CONNECT FIRST GENERATOR
    AddStep([]
            {
                const core::Pointd2D p0(900, 250);
                return new StepGameConnectStructIntro(p0);
            });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const GameObject * gen1 = GetObjectInCell(cellEneGen1);
                const GameObject * gen2 = GetObjectInCell(cellMatGen1);

                core::Pointd2D p0(0, 250);
                Cell2D target;

                // conquered energy generator first
                if(gen1->GetFaction() != NO_FACTION)
                {
                    mFirstGenConqueredIsEnergy = true;
                    p0.x = 1100;
                    target = Cell2D(6, 13);
                }
                // conquered material generator first
                else
                {
                    mFirstGenConqueredIsEnergy = false;
                    p0.x = 400;
                    target = Cell2D(13, 7);
                }

                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            core::Pointd2D p0(500, 250);

            if(mFirstGenConqueredIsEnergy)
                p0.x = 1000;

            const Cell2D target = mFirstGenConqueredIsEnergy ? Cell2D(6, 7) : Cell2D(6, 7);

            return new StepGameConquerCellsEnd(game, isoMap, unit, target, p0);
        });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT SECOND GENERATOR
    AddStep([game]
        {
            core::Pointd2D p0(700, 200);
            return new StepGameConnectStructIntro(p0, "TUT_GAME_CONNECT_STRUCT_1");
        });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
        {
            const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();

            core::Pointd2D p0(0, 200);
            Cell2D target;

            if(mFirstGenConqueredIsEnergy)
            {
                p0.x = 400;
                target = Cell2D(7, 7);
            }
            else
            {
                p0.x = 1100;
                target = Cell2D(6, 8);
            }

            return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
        });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(650, 250);

                Cell2D target;

                if(mFirstGenConqueredIsEnergy)
                    target = Cell2D(13, 7);
                else
                    target = Cell2D(6, 13);

                return new StepGameConquerCellsEnd(game, isoMap, unit, target, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::MOVE); });
    // CONQUER SECOND GENERATOR
    AddStep([this, game]
        {
            const GameObject * gen;
            const char * text;
            core::Pointd2D p0(0, 200);

            if(mFirstGenConqueredIsEnergy)
            {
                gen = GetObjectInCell(cellMatGen1);
                text = "TUT_GAME_CONQUER_MAT_GEN";
                p0.x = 400;
            }
            else
            {
                gen = GetObjectInCell(cellEneGen1);
                text = "TUT_GAME_CONQUER_ENE_GEN";
                p0.x = 1100;
            }

            return new StepGameConquerGeneratorIntro(gen, text, p0);
        });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::MOVE); });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const GameObject * gen = mFirstGenConqueredIsEnergy ?
                                         GetObjectInCell(cellMatGen1) : GetObjectInCell(cellEneGen1);
                const sgl::core::Pointd2D p0(550, 250);
                return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
            });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD BARRACKS
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1000, 625);
                return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_BARRACKS_1", p0);
            });
    AddStep([hud] { return new StepGameBuildUnitEnd(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
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
    AddStep([panelActions]
        {
            return new StepGameBuildUnitStart(panelActions,
                                              PanelObjectActions::BTN_BUILD_UNIT_BARRACKS);
        });
    AddStep([hud] { return new StepGameBuildUnitEnd(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([this]
        {
            const auto barracks = static_cast<Structure *>(GetObjectInCell(cellBarracks));
            return new StepDelay(barracks->GetTimeBuildUnit());
        });
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const core::Pointd2D p0(1300, 450);
                return new StepGameUnit(game, isoMap, unit, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE SOLDIER NEAR TREES
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indSoldier1);
            const Cell2D target(15, 11);
            const core::Pointd2D p0(1100, 650);
            return new StepGameMoveUnitSimple(game, unit, isoMap, target, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // ATTACK TREES WITH SOLDIER
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([this]
            {
                const float health = 15.f;
                GameObject * enemy = GetObjectInCell(cellTarget1);
                return new StepGameSetObjectHealth(enemy, health);
            });
    AddStep([]
        {
            const core::Pointd2D p0(400, 600);
            return new StepGameSingleInfo(p0, "TUT_GAME_ATTACK_1");
        });
    AddStep([panelActions]
        {
            const core::Pointd2D p0(450, 650);
            return new StepGameUnitAttackIcon(panelActions, p0);
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
                const Cell2D targetTL(13, 13);
                const Cell2D targetBR(16, 16);

                return new StepGamePanelHit(isoMap, targetBR, targetTL, p0);
            });
    AddStep([local]
            {
                auto unit = local->GetUnit(indSoldier1);
                return new StepGameSetObjectPerfectShot(unit, true);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const core::Pointd2D p0(250, 500);
                return new StepGameUnitAttackSimple(game, unit, isoMap, cellTarget1, p0);
            });
    AddStep([this]
            {
                GameObject * enemy = GetObjectInCell(cellTarget1);
                return new StepGameWaitEnemyKilled(enemy, GetGameMap());
            });
    AddStep([local]
            {
                auto unit = local->GetUnit(indSoldier1);
                return new StepGameSetObjectPerfectShot(unit, false);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE SOLDIER TO DIAMONDS AND COLLECT THEM
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const Cell2D target(23, 9);
                const core::Pointd2D p0(450, 200);
                return new StepGameMoveUnitToArea(unit, isoMap, areaDiamondsTL, areaDiamondsBR,
                                                  target, "TUT_GAME_MOVE_UNIT_4", p0);
            });
    AddStep([isoMap, gameMap, local, game]
        {
            const auto unit = local->GetUnit(indSoldier1);
            const Cell2D areaTL(areaDiamondsTL.row + 1, areaDiamondsTL.col);
            const core::Pointd2D p0(400, 100);

            return new StepGameCollectObjects(unit, ObjectData::TYPE_DIAMONDS, game, gameMap, isoMap,
                                              areaTL, areaDiamondsBR, "TUT_GAME_COLLECT_DIAMONDS", p0);
        });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    // MOVE CAMERA OVER BLOBS
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([isoMap]
        {
            const float speed = 800.f;
            const Cell2D cell(14, 14);
            return new StepGameMoveCameraOverCell(cell, isoMap, speed);
        });
    // SELECT WORKER AND COLLECT BLOBS
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(1300, 450);
                return new StepGameUnit(game, isoMap, unit, p0);
            });
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indWorker1);
                const Cell2D target(25, 12);
                const core::Pointd2D p0(900, 550);
                return new StepGameMoveUnitToArea(unit, isoMap, areaBlobsTL, areaBlobsBR,
                                                  target, "TUT_GAME_MOVE_UNIT_5", p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([isoMap, gameMap, local, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const Cell2D areaTL(areaBlobsTL.row, areaBlobsTL.col + 1);
                const core::Pointd2D p0(500, 150);

                return new StepGameCollectObjects(unit, ObjectData::TYPE_BLOBS, game, gameMap, isoMap,
                                                  areaTL, areaBlobsBR, "TUT_GAME_COLLECT_BLOBS", p0);
            });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepDelay(0.5f); });
    // UPGRADE UNIT
    AddStep([panelActions]
            {
                core::Pointd2D p0(900, 150);
                return new StepGameUpgradeIntro(panelActions, "TUT_GAME_UPGRADE_1b", p0);
            });
    AddStep([hud] { return new StepGameUpgradeUnit(hud, false); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    // CONQUER SECOND MATERIAL GENERATOR WITH SOLDIER
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game]
            {
                const int movX = -100;
                const int movY = -100;
                return new StepGameMoveCamera(movX, movY);
            });
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const core::Pointd2D p0(1300, 450);
                return new StepGameUnit(game, isoMap, unit, p0);
            });
    AddStep([this, local]
            {
                const GameObject * gen = GetObjectInCell(cellMatGen2);

                return new StepGameConquerMaterialGenIntro(gen);
            });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::MOVE); });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const GameObject * gen = GetObjectInCell(cellMatGen2);
                const core::Pointd2D p0(1100, 350);
                return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
            });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    // MAKE SURE SOLDIER'S ENERGY IS FULL (it will be needed for connection)
    AddStep([local]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const float val = unit->GetMaxEnergy();
                return new StepGameSetObjectEnergy(unit, val);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT SECOND MATERIAL GENERATOR
    AddStep([]
            {
                const core::Pointd2D p0(1150, 150);
                return new StepGameConnectStructIntro(p0);
            });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(900, 250);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(23, 4);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const core::Pointd2D p0(700, 250);
                const Cell2D cellEnd(15, 5);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    // CONQUER SECOND ENERGY GENERATOR WITH WORKER
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game]
            {
                const int movX = 0;
                const int movY = 250;
                return new StepGameMoveCamera(movX, movY);
            });
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(1300, 450);
                return new StepGameUnit(game, isoMap, unit, p0);
            });
    AddStep([this, local]
            {
                const core::Pointd2D p0(1100, 400);
                const GameObject * gen = GetObjectInCell(cellEneGen2);

                return new StepGameConquerEnergyGenIntro(gen, p0);
            });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::MOVE); });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const GameObject * gen = GetObjectInCell(cellEneGen2);
                const core::Pointd2D p0(1000, 150);
                return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
            });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT SECOND ENERGY GENERATOR WITH WORKER 1
    AddStep([]
            {
                const core::Pointd2D p0(1150, 150);
                return new StepGameConnectStructIntro(p0);
            });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(1000, 200);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(22, 15);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(1200, 150);
                const Cell2D cellEnd(18, 9);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // CONTINUE TO CONNECT SECOND ENERGY GENERATOR WITH SOLDIER 1
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const core::Pointd2D p0(1300, 450);
                return new StepGameUnit(game, isoMap, unit, p0);
            });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(1000, 200);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(18, 8);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const core::Pointd2D p0(1200, 150);
                const Cell2D cellEnd(15, 8);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD RESEARCH CENTER
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(1300, 450);
                return new StepGameUnit(game, isoMap, unit, p0);
            });
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1100, 450);
                return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_RES_CEN_1", p0);
            });
    AddStep([hud]
            {
                const int indStruct = 0;
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_RES_CEN_2", nullptr,
                                                  catTech, indStruct);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(1100, 500);
                return new StepGameBuildTowerEnd(isoMap, unit, cellResCenter, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // SETUP RESEARCH
    AddStep([this, game, isoMap]
            {
                const core::Pointd2D p0(1100, 550);
                const GameObject * obj = GetObjectInCell(cellResCenter);
                return new StepGameSelectObject(game, isoMap, obj, "TUT_GAME_RES_CEN_1", p0);
            });
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1000, 650);
                return new StepGameSetupResearchIcon(panelActions, p0);
            });
    AddStep([hud]
            {
                return new StepGameSetupResearch(hud);
            });
    // CLEAR SELECTION AND END TURN
    AddStep([gs]
            {
                return new StepGameClearSelection(gs);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // EXPLAIN RESOURCES BAR
    AddStep([hud]
            {
                return new StepGameResourcesBar(hud);
            });
    // SELECT SOLDIER AND MOVE TO RIGHT CORNER
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const core::Pointd2D p0(1300, 450);
                return new StepGameUnit(game, isoMap, unit, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const Cell2D target(14, 19);
                const core::Pointd2D p0(1150, 500);
                return new StepGameMoveUnitSimple(game, unit, isoMap, target, p0);
            });
    // SELECT WORKER AND CONQUER THIRD MATERIAL GENERATOR
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([] { return new StepGameMoveCamera(-200.f, -100.f); });
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(900, 150);
                return new StepGameUnit(game, isoMap, unit, p0);
            });
    AddStep([this, local]
            {
                const GameObject * gen = GetObjectInCell(cellMatGen3);
                return new StepGameConquerMaterialGenIntro(gen);
            });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::MOVE); });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const GameObject * gen = GetObjectInCell(cellMatGen3);
                const core::Pointd2D p0(1100, 450);
                return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT THIRD MATERIAL GENERATOR WITH WORKER
    AddStep([]
            {
                const core::Pointd2D p0(1100, 350);
                return new StepGameConnectStructIntro(p0);
            });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(1100, 500);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(14, 24);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(1050, 350);
                const Cell2D cellEnd(10, 18);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // UPGRADE WORKER
    AddStep([panelActions, game]
            {
                core::Pointd2D p0(1000, 300);
                return new StepGameUpgradeIntro(panelActions, "TUT_GAME_UPGRADE_1b", p0);
            });
    AddStep([hud] { return new StepGameUpgradeUnitFree(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT SOLDIER AND CONTINUE THIRD MATERIAL GENERATOR CONNECTION
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const core::Pointd2D p0(1300, 450);
                return new StepGameUnit(game, isoMap, unit, p0);
            });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(1100, 500);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(9, 18);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const core::Pointd2D p0(1150, 250);
                const Cell2D cellEnd(6, 16);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // UPGRADE SOLDIER
    AddStep([panelActions]
            {
                core::Pointd2D p0(1100, 200);
                return new StepGameUpgradeIntro(panelActions, "TUT_GAME_UPGRADE_1b", p0);
            });
    AddStep([hud] { return new StepGameUpgradeUnit(hud, false); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // SELECT SOLDIER AND CONQUER ENERGY GENERATOR 3
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const float speed = 600.f;
                return new StepGameMoveCameraOverObject(unit, speed);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const core::Pointd2D p0(1100, 250);
                return new StepGameUnit(game, isoMap, unit, p0);
            });
    AddStep([this, local]
            {
                const GameObject * gen = GetObjectInCell(cellEneGen3);
                const core::Pointd2D p0(1200, 350);
                return new StepGameConquerEnergyGenIntro(gen, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const GameObject * gen = GetObjectInCell(cellEneGen3);
                const core::Pointd2D p0(1200, 350);
                return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
            });
    // SELECT WORKER AND CONNECT THIRD ENERGY GENERATOR
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(900, 200);
                return new StepGameUnit(game, isoMap, unit, p0);
            });
    AddStep([]
            {
                const core::Pointd2D p0(1100, 250);
                return new StepGameConnectStructIntro(p0);
            });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(1100, 300);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(6, 18);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(110, 400);
                const Cell2D cellEnd(5, 24);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL ON RIGHT SIDE WITH WORKER
    AddStep([] { return new StepGameWallBuildIntro(); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelActions] { return new StepGameWallBuildIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
                const Cell2D target(2, 33);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([isoMap, local, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const Cell2D cellEnd(3, 33);
                const core::Pointd2D p0(1100, 250);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD GATE
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1100, 400);
                return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_GATE_1", p0);
            });
    AddStep([hud]
            {
                const int indStruct = 2;
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_GATE_2", catDefenses, indStruct);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(1100, 450);
                const Cell2D target(4, 33);
                return new StepGameBuildTowerEnd(isoMap, unit, target, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT SOLDIER AND CONNECT GATE
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const core::Pointd2D p0(1200, 250);
                return new StepGameUnit(game, isoMap, unit, p0);
            });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(1200, 300);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(4, 27);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const core::Pointd2D p0(1200, 350);
                const Cell2D cellEnd(4, 32);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // SELECT RESEARCH CENTER AND UNLOCK UPGRADE
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this]
            {
                const GameObject * obj = GetObjectInCell(cellResCenter);
                const float speed = 600.f;
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
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1000, 650);
                return new StepGameTechTreeIcon(panelActions, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([hud] { return new StepGameTechTreeDialog(hud, TECH_UP_BASE_IMPROVE_1, true); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER AND BUILD MORE WALL
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(100, 600);
                return new StepGameQuickUnitButton(hud, indWorker1, nullptr, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelActions] { return new StepGameWallBuildIcon(panelActions); });
    AddStep([isoMap, local, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const Cell2D cellEnd(5, 33);
                const core::Pointd2D p0(1100, 250);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD DEFENSIVE TOWER
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1000, 250);
                return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
            });
    AddStep([hud]
            {
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_DTOWER_4", catDefenses, structDefTower);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(1000, 350);
                return new StepGameBuildTowerEnd(isoMap, unit, cellTower1, p0);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // MOVE WORKER AWAY FROM WALL
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game]
        {
            const int movX = -300;
            const int movY = 0;
            return new StepGameMoveCamera(movX, movY);
        });
    AddStep([this, local, isoMap, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const Cell2D target(20, 28);
            const core::Pointd2D p0(400, 500);
            return new StepGameMoveUnitSimple(game, unit, isoMap, target, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // ADD ENEMY NEAR GATE 1
    AddStep([]
        {
            const core::Pointd2D p0(1100, 400);
            return new StepGameEnemyIntro(p0);
        });
    AddStep([this, playerAI]
        {
            return new StepGameAddEnemy(GetGameMap(), playerAI, ObjectData::TYPE_UNIT_SCOUT1,
                                        cellEnemy1, true);
        });
    // CENTER VIEW ON SOLDIER
    AddStep([hud]
        {
            const sgl::core::Pointd2D p0(200, 700);
            return new StepGameQuickUnitButton(hud, indSoldier1, nullptr, p0);
        });
    // DISABLE PERFECT SHOT AND FATAL HIT
    AddStep([local]
        {
            auto unit = local->GetUnit(indSoldier1);
            return new StepGameSetObjectPerfectShot(unit, false);
        });
    AddStep([local]
        {
            auto unit = local->GetUnit(indSoldier1);
            return new StepGameSetObjectFatalHit(unit, false);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // ATTACK ENEMY WITH SOLDIER
    AddStep([]
            {
                const core::Pointd2D p0(700, 200);
                return new StepGameSingleInfo(p0, "TUT_GAME_ATTACK_1b");
            });
    AddStep([panelActions]
            {
                const core::Pointd2D p0(700, 250);
                return new StepGameUnitAttackIcon(panelActions, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const core::Pointd2D p0(700, 250);
                return new StepGameUnitAttackContinue(game, unit, isoMap, cellEnemy1, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // ENABLE TOWER'S PERFECT SHOT
    AddStep([this]
            {
                auto tower = GetObjectInCell(cellTower1);
                return new StepGameSetObjectPerfectShot(tower, true);
            });
    // LOWER ENEMY HEALTH, SET ATTACK TO BURST AND DISABLE FATAL HIT
    AddStep([this]
            {
                const float health = 20.f;
                GameObject * enemy = GetObjectInCell(cellEnemy1);
                return new StepGameSetObjectHealth(enemy, health);
            });
    AddStep([this]
            {
                auto attacker = GetObjectInCell(cellEnemy1);
                return new StepGameSetObjectAttackMode(attacker, ATT_BURST_SHOT);
            });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    // MAKE ENEMY SHOOT
    AddStep([this, playerAI, local]
            {
                auto attacker = static_cast<Unit *>(GetObjectInCell(cellEnemy1));
                auto target = local->GetUnit(indSoldier1);
                return new StepGameMakeEnemyAttack(attacker, target, playerAI);
            });
    // WAIT TURN END
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE SOLDIER TO EXPLORE DIAMONDS AND BLOBS
    AddStep([]
            {
                const core::Pointd2D p0(1000, 250);
                return new StepGameSingleInfo(p0, "TUT_GAME_QUICK_SEL_1");
            });
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const core::Pointd2D p0(1300, 350);
                return new StepGameUnit(game, isoMap, unit, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const Cell2D target(5, 42);
                const core::Pointd2D p0(1150, 500);
                return new StepGameMoveUnitSimple(game, unit, isoMap, target, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 1 AND BUILD WALL
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(100, 600);
                return new StepGameQuickUnitButton(hud, indWorker1, nullptr, p0);
            });
    AddStep([panelActions] { return new StepGameWallBuildIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
                const Cell2D target(16, 31);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([isoMap, local, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const Cell2D cellEnd(12, 31);
                const core::Pointd2D p0(1100, 150);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE VIEW BACK TO BASE
    AddStep([panelTurn, game]
            {
                const core::Pointd2D p0(50, 700);
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
                return new StepGameBuildUnitStart(panelActions,
                                                  PanelObjectActions::BTN_BUILD_UNIT_BASE);
            });
    AddStep([hud] { return new StepGameBuildUnitEnd(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([localBase] { return new StepDelay(localBase->GetTimeBuildUnit()); });
    // SELECT WORKER UNIT 2 AND MOVE DOWN
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indWorker2);
                const core::Pointd2D p0(1200, 450);
                return new StepGameUnit(game, isoMap, unit, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game]
            {
                const int movX = 0;
                const int movY = 500;
                return new StepGameMoveCamera(movX, movY);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker2);
                const Cell2D target(26, 12);
                const core::Pointd2D p0(550, 650);
                return new StepGameMoveUnitSimple(game, unit, isoMap, target, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // START TO BUILD WALL WITH WORKER 2
    AddStep([panelActions] { return new StepGameWallBuildIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
                const Cell2D target(28, 12);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([isoMap, local, game]
            {
                const auto unit = local->GetUnit(indWorker2);
                const Cell2D cellEnd(28, 15);
                const core::Pointd2D p0(1200, 500);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 1 AND BUILD DEFENSIVE TOWER
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(100, 600);
                return new StepGameQuickUnitButton(hud, indWorker1, nullptr, p0);
            });
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1000, 250);
                return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
            });
    AddStep([hud]
            {
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_DTOWER_4", catDefenses, structDefTower);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(100, 700);
                return new StepGameBuildTowerEnd(isoMap, unit, cellTower2, p0);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT SOLDIER 1 AND COLLECT BLOBS
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(100, 600);
                return new StepGameQuickUnitButton(hud, indSoldier1, nullptr, p0);
            });
    AddStep([isoMap, gameMap, local, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const Cell2D areaTL(2, 46);
                const Cell2D areaBR(4, 48);
                const core::Pointd2D p0(500, 150);

                return new StepGameCollectObjects(unit, ObjectData::TYPE_BLOBS, game, gameMap, isoMap,
                                                  areaTL, areaBR, "TUT_GAME_COLLECT_BLOBS", p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // COLLECT DIAMONDS WITH SOLDIER 1
    AddStep([isoMap, gameMap, local, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const Cell2D areaTL(7, 41);
                const Cell2D areaBR(9, 42);
                const core::Pointd2D p0(500, 150);

                return new StepGameCollectObjects(unit, ObjectData::TYPE_DIAMONDS, game, gameMap, isoMap,
                                                  areaTL, areaBR, "TUT_GAME_COLLECT_DIAMONDS", p0);
            });
    // SELECT WORKER 1 AND CONNECT WALL TO BASE
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(100, 600);
                return new StepGameQuickUnitButton(hud, indWorker1, nullptr, p0);
            });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(1200, 450);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(15, 30);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(1200, 250);
                const Cell2D cellEnd(15, 27);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    // BUILD GATE WITH WORKER 2
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(100, 600);
                return new StepGameQuickUnitButton(hud, indWorker2, nullptr, p0);
            });
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1100, 400);
                return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_GATE_1", p0);
            });
    AddStep([hud]
            {
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_GATE_2", catDefenses, structGate);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker2);
                const core::Pointd2D p0(1100, 450);
                const Cell2D target(28, 16);
                return new StepGameBuildTowerEnd(isoMap, unit, target, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD DEFENSIVE TOWER WITH WORKER 2
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1000, 250);
                return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
            });
    AddStep([hud]
            {
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_DTOWER_4", catDefenses, structDefTower);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indWorker2);
                const core::Pointd2D p0(1000, 350);
                return new StepGameBuildTowerEnd(isoMap, unit, cellTower3, p0);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 1 AND CONTINUE TO BUILD WALL
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(100, 600);
                return new StepGameQuickUnitButton(hud, indWorker1, nullptr, p0);
            });
    AddStep([panelActions] { return new StepGameWallBuildIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
                const Cell2D target(18, 30);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([isoMap, local, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const Cell2D cellEnd(18, 31);
                const core::Pointd2D p0(1200, 500);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD GATE WITH WORKER 1
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1100, 400);
                return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_GATE_1", p0);
            });
    AddStep([hud]
            {
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_GATE_2", catDefenses, structGate);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(1100, 450);
                const Cell2D target(19, 30);
                return new StepGameBuildTowerEnd(isoMap, unit, target, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // CONTINUE TO BUILD WALL WITH WORKER 1
    AddStep([panelActions] { return new StepGameWallBuildIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
                const Cell2D target(20, 30);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([isoMap, local, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const Cell2D cellEnd(21, 30);
                const core::Pointd2D p0(1200, 500);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT SOLDIER AND CONQUER ENERGY GENERATOR 4
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(100, 600);
                return new StepGameQuickUnitButton(hud, indSoldier1, nullptr, p0);
            });
    AddStep([this, local]
            {
                const GameObject * gen = GetObjectInCell(cellEneGen4);
                const core::Pointd2D p0(1200, 350);
                return new StepGameConquerEnergyGenIntro(gen, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const GameObject * gen = GetObjectInCell(cellEneGen4);
                const core::Pointd2D p0(1200, 350);
                return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
            });
    // CONQUER CELLS WITH SOLDIER 1
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(1200, 450);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(12, 41);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const core::Pointd2D p0(1200, 250);
                const Cell2D cellEnd(12, 36);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    // UPGRADE SOLDIER 1
    AddStep([panelActions, game]
            {
                core::Pointd2D p0(1000, 300);
                return new StepGameUpgradeIntro(panelActions, "TUT_GAME_UPGRADE_1b", p0);
            });
    AddStep([hud] { return new StepGameUpgradeUnitFree(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER CELLS WITH SOLDIER 1
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(1200, 450);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(12, 35);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const core::Pointd2D p0(1200, 250);
                const Cell2D cellEnd(12, 32);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    // SELECT WORKER 1 AND BUILD DEFENSIVE TOWER
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(1300, 450);
                return new StepGameUnit(game, isoMap, unit, p0);
            });
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1000, 250);
                return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
            });
    AddStep([hud]
            {
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_DTOWER_4", catDefenses, structDefTower);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(100, 450);
                return new StepGameBuildTowerEnd(isoMap, unit, cellTower4, p0);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 2
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(100, 600);
                return new StepGameQuickUnitButton(hud, indWorker2, nullptr, p0);
            });
    // BUILD WALL WITH WORKER 2
    AddStep([panelActions] { return new StepGameWallBuildIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
                const Cell2D target(28, 18);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([isoMap, local, game]
            {
                const auto unit = local->GetUnit(indWorker2);
                const Cell2D cellEnd(27, 20);
                const core::Pointd2D p0(1200, 500);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1000, 250);
                return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
            });
    AddStep([hud]
            {
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_DTOWER_4", catDefenses, structDefTower);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indWorker2);
                const core::Pointd2D p0(100, 700);
                return new StepGameBuildTowerEnd(isoMap, unit, cellTower5, p0);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER CELLS WITH WORKER 2
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(1200, 450);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(26, 18);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker2);
                const core::Pointd2D p0(1200, 250);
                const Cell2D cellEnd(23, 18);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    // SELECT WORKER 1 AND BUILD DEFENSIVE TOWER
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(100, 600);
                return new StepGameQuickUnitButton(hud, indWorker1, nullptr, p0);
            });
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1000, 250);
                return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
            });
    AddStep([hud]
            {
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_DTOWER_4", catDefenses, structDefTower);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(100, 700);
                return new StepGameBuildTowerEnd(isoMap, unit, cellTower6, p0);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT SOLDIER AND MOVE
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(100, 600);
                return new StepGameQuickUnitButton(hud, indSoldier1, nullptr, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game]
            {
                const int movX = -700;
                const int movY = 0;
                return new StepGameMoveCamera(movX, movY);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const Cell2D target(26, 27);
                const core::Pointd2D p0(300, 300);
                return new StepGameMoveUnitSimple(game, unit, isoMap, target, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // MOVE SOLDIER 1
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game]
            {
                const int movX = -700;
                const int movY = 0;
                return new StepGameMoveCamera(movX, movY);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const Cell2D target(34, 12);
                const core::Pointd2D p0(500, 150);
                return new StepGameMoveUnitSimple(game, unit, isoMap, target, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER MATERIAL GENERATOR WITH SOLDIER
    AddStep([this, local]
            {
                const GameObject * gen = GetObjectInCell(cellMatGen4);

                return new StepGameConquerMaterialGenIntro(gen);
            });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::MOVE); });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const GameObject * gen = GetObjectInCell(cellMatGen4);
                const core::Pointd2D p0(1100, 350);
                return new StepGameConquerStructSimple(game, unit, gen, isoMap, p0);
            });
    // SELECT WORKER 1 AND BUILD WALL
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(100, 600);
                return new StepGameQuickUnitButton(hud, indWorker1, nullptr, p0);
            });
    AddStep([panelActions] { return new StepGameWallBuildIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
                const Cell2D target(11, 33);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([isoMap, local, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const Cell2D cellEnd(7, 33);
                const core::Pointd2D p0(1200, 500);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 2
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(100, 600);
                return new StepGameQuickUnitButton(hud, indWorker2, nullptr, p0);
            });
    // CONQUER CELLS WITH WORKER 2
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(1200, 450);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(23, 19);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker2);
                const core::Pointd2D p0(1200, 250);
                const Cell2D cellEnd(23, 23);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    // UPGRADE WORKER 2
    AddStep([panelActions]
            {
                core::Pointd2D p0(900, 150);
                return new StepGameUpgradeIntro(panelActions, "TUT_GAME_UPGRADE_1b", p0);
            });
    AddStep([hud] { return new StepGameUpgradeUnit(hud, false); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
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
    AddStep([panelActions] { return new StepGameMissionGoalsIcon(panelActions, false); });
    AddStep([hud]
        {
            const int goal = 0;
            return new StepGameSecondaryMissionGoal(hud, goal);
        });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 1
    AddStep([hud]
        {
            const sgl::core::Pointd2D p0(100, 600);
            return new StepGameQuickUnitButton(hud, indWorker1, nullptr, p0);
        });
    // MOVE CAMERA
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game]
        {
            const int movX = -700;
            const int movY = -300;
            return new StepGameMoveCamera(movX, movY);
        });
    // MOVE WORKER 1
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const Cell2D target(17, 10);
                const core::Pointd2D p0(500, 350);
                return new StepGameMoveUnitSimple(game, unit, isoMap, target, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT SOLDIER 1 AND CONQUER CELLS
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(100, 600);
                return new StepGameQuickUnitButton(hud, indSoldier1, nullptr, p0);
            });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(600, 250);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(38, 12);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const core::Pointd2D p0(600, 200);
                const Cell2D cellEnd(33, 12);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    // SELECT WORKER 2 AND BUILD DEFENSIVE TOWER
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(100, 600);
                return new StepGameQuickUnitButton(hud, indWorker2, nullptr, p0);
            });
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1000, 250);
                return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
            });
    AddStep([hud]
            {
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_DTOWER_4", catDefenses, structDefTower);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indWorker2);
                const core::Pointd2D p0(100, 700);
                return new StepGameBuildTowerEnd(isoMap, unit, cellTower7, p0);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL WITH WORKER 2
    AddStep([panelActions] { return new StepGameWallBuildIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
                const Cell2D target(24, 24);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([isoMap, local, game]
            {
                const auto unit = local->GetUnit(indWorker2);
                const Cell2D cellEnd(23, 26);
                const core::Pointd2D p0(1200, 500);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD TOWER WITH WORKER 2
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1000, 250);
                return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
            });
    AddStep([hud]
            {
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_DTOWER_4", catDefenses, structDefTower);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indWorker2);
                const core::Pointd2D p0(100, 700);
                return new StepGameBuildTowerEnd(isoMap, unit, cellTower8, p0);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT SOLDIER 1
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(100, 600);
                return new StepGameQuickUnitButton(hud, indSoldier1, nullptr, p0);
            });
    // CONQUER CELLS WITH SOLDIER
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(600, 250);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(32, 12);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const core::Pointd2D p0(600, 200);
                const Cell2D cellEnd(29, 12);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    // SELECT WORKER 1
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(100, 600);
                return new StepGameQuickUnitButton(hud, indWorker1, nullptr, p0);
            });
    // MOVE CAMERA
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game]
            {
                const int movX = -200;
                const int movY = 0;
                return new StepGameMoveCamera(movX, movY);
            });
    // BUILD TOWER WITH WORKER 1
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1000, 250);
                return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
            });
    AddStep([hud]
            {
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_DTOWER_4", catDefenses, structDefTower);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(700, 700);
                return new StepGameBuildTowerEnd(isoMap, unit, cellTower9, p0);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL WITH WORKER 1
    AddStep([panelActions] { return new StepGameWallBuildIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
                const Cell2D target(28, 8);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([isoMap, local, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const Cell2D cellEnd(28, 5);
                const core::Pointd2D p0(1200, 500);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD GATE WITH WORKER 1
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1100, 400);
                return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_GATE_1", p0);
            });
    AddStep([hud]
            {
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_GATE_2", catDefenses, structGate);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(1100, 450);
                const Cell2D target(28, 4);
                return new StepGameBuildTowerEnd(isoMap, unit, target, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 2
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(100, 600);
                return new StepGameQuickUnitButton(hud, indWorker2, nullptr, p0);
            });
    // MOVE CAMERA
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game]
            {
                const int movX = -700;
                const int movY = -250;
                return new StepGameMoveCamera(movX, movY);
            });
    // MOVE WORKER 2
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker2);
                const Cell2D target(30, 4);
                const core::Pointd2D p0(500, 350);
                return new StepGameMoveUnitSimple(game, unit, isoMap, target, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER CELLS WITH WORKER 2
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(600, 250);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(27, 3);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker2);
                const core::Pointd2D p0(600, 200);
                const Cell2D cellEnd(25, 3);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    // SELECT WORKER 1
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(1300, 450);
                return new StepGameUnit(game, isoMap, unit, p0);
            });
    // CONTINUE BUILD WALL WITH WORKER 1
    AddStep([panelActions] { return new StepGameWallBuildIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayWall()->GetCellStart();
                const Cell2D target(28, 3);
                return new StepGameWallBuildStart(game, isoMap, cellStart, target);
            });
    AddStep([isoMap, local, game]
            {
                const auto unit = local->GetUnit(indWorker1);
                const Cell2D cellEnd(28, 1);
                const core::Pointd2D p0(1100, 250);
                return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD TOWER WITH WORKER 1
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1000, 250);
                return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
            });
    AddStep([hud]
            {
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_DTOWER_3",
                                                  "TUT_GAME_BUILD_DTOWER_4", catDefenses, structDefTower);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap]
            {
                const auto unit = local->GetUnit(indWorker1);
                const core::Pointd2D p0(700, 700);
                return new StepGameBuildTowerEnd(isoMap, unit, cellTower10, p0);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // END TURN
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    // SELECT RESEARCH CENTER AND UNLOCK UPGRADE
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this]
            {
                const GameObject * obj = GetObjectInCell(cellResCenter);
                const float speed = 600.f;
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
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1000, 650);
                return new StepGameTechTreeIcon(panelActions, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([hud] { return new StepGameTechTreeDialog(hud, TECH_UP_TRADING_POST, false); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 2
    AddStep([hud]
            {
                const sgl::core::Pointd2D p0(100, 600);
                return new StepGameQuickUnitButton(hud, indWorker2, nullptr, p0);
            });
    // MOVE CAMERA
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game]
            {
                const int movX = 200;
                const int movY = -100;
                return new StepGameMoveCamera(movX, movY);
            });
    // BUILD TRADING POST
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1100, 450);
                return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_TRAD_POST_1", p0);
            });
    AddStep([hud]
            {
                return new StepGameBuildStructure(hud, nullptr, "TUT_GAME_BUILD_TRAD_POST_2",
                                                  catGeneric, structTradingPost);
            });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indWorker2);
                const core::Pointd2D p0(1100, 250);
                return new StepGameBuildTowerEnd(isoMap, unit, cellTradingPost, p0);
            });
    AddStep([] { return new StepDelay(0.5f); });
}

TutorialGame2::~TutorialGame2()
{
    auto game = GetScreen()->GetGame();

    // reset flags for unit soldier 1
    const Player * local = game->GetPlayerByIndex(0);
    auto unit1 = local->GetUnit(indSoldier1);

    if(unit1 != nullptr)
    {
        unit1->SetPerfectShot(false);
        unit1->SetFatalHit(true);
    }

    // reset flags for tower 1
    auto tower1 = GetObjectInCell(cellTower1);

    if(tower1 != nullptr)
        tower1->SetPerfectShot(false);
}

} // namespace game
