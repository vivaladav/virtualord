#include "Tutorial/TutorialGame2.h"

#include "CameraMapController.h"
#include "Game.h"
#include "GameMap.h"
#include "Player.h"
#include "GameObjects/Base.h"
#include "GameObjects/Unit.h"
#include "Indicators/OverlayCellConquest.h"
#include "Screens/ScreenGame.h"
#include "Tutorial/StepAISetActive.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepGameIntro2.h"
#include "Tutorial/StepGameBuildStructIntro.h"
#include "Tutorial/StepGameBuildStructure.h"
#include "Tutorial/StepGameBuildTowerEnd.h"
#include "Tutorial/StepGameBuildUnitEnd.h"
#include "Tutorial/StepGameBuildUnitStart.h"
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
#include "Tutorial/StepGameMoveCameraOverCell.h"
#include "Tutorial/StepGameMoveCameraOverObject.h"
#include "Tutorial/StepGameMoveUnitSimple.h"
#include "Tutorial/StepGameMoveUnitToArea.h"
#include "Tutorial/StepGamePanelHit.h"
#include "Tutorial/StepGameSelectObject.h"
#include "Tutorial/StepGameSetCollectableGeneratorTurns.h"
#include "Tutorial/StepGameSetCollectableUnits.h"
#include "Tutorial/StepGameSetObjectEnergy.h"
#include "Tutorial/StepGameSetObjectHealth.h"
#include "Tutorial/StepGameSetObjectPerfectShot.h"
#include "Tutorial/StepGameSetSelectionDefaultAction.h"
#include "Tutorial/StepGameSetSelectionActiveAction.h"
#include "Tutorial/StepGameSetupResearchIcon.h"
#include "Tutorial/StepGameSingleInfo.h"
#include "Tutorial/StepGameUnit.h"
#include "Tutorial/StepGameUnitAttackBurst.h"
#include "Tutorial/StepGameUnitAttackIcon.h"
#include "Tutorial/StepGameUnitAttackSimple.h"
#include "Tutorial/StepGameUnitConquerCellsIcon.h"
#include "Tutorial/StepGameUpgradeIntro.h"
#include "Tutorial/StepGameUpgradeUnit.h"
#include "Tutorial/StepGameWaitEnemyKilled.h"
#include "Tutorial/StepGameWaitTurn.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/GameHUD.h"
#include "Widgets/PanelObjectActions.h"

namespace
{
using namespace game;

constexpr unsigned int indWorker1 = 0;
constexpr unsigned int indSoldier1 = 1;

constexpr int turnsCollGenMin = 1;
constexpr int turnsCollGenMax = 1;
constexpr int turnsCollGenMin2 = 5;
constexpr int turnsCollGenMax2 = 15;
constexpr int collectablesMin = 2;
constexpr int collectablesMax = 4;

const Cell2D cellEneGen1(6, 15);
const Cell2D cellEneGen2(23, 17);
const Cell2D cellMatGen1(15, 7);
const Cell2D cellMatGen2(24,3);
const Cell2D cellBarracks(8, 9);
const Cell2D cellTarget1(15, 13);
const Cell2D cellResCenter(11, 6);

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
            return new StepGameUnit(game, isoMap, unit);
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
                return new StepGameUnit(game, isoMap, unit);
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
            return new StepGameUnitAttackBurst(panelShot, p0);
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
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA OVER BLOBS
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
                return new StepGameUnit(game, isoMap, unit);
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
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER SECOND MATERIAL GENERATOR WITH SOLDIER
    AddStep([game]
            {
                const int movX = -100;
                const int movY = -100;
                return new StepGameMoveCamera(movX, movY);
            });
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indSoldier1);
                return new StepGameUnit(game, isoMap, unit);
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
    // CONQUER SECOND ENERGY GENERATOR WITH WORKER
    AddStep([game]
            {
                const int movX = 200;
                const int movY = 0;
                return new StepGameMoveCamera(movX, movY);
            });
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indWorker1);
                return new StepGameUnit(game, isoMap, unit);
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
    // CONNECT SECOND MATERIAL GENERATOR
    AddStep([game]
            {
                const int movX = -100;
                const int movY = -100;
                return new StepGameMoveCamera(movX, movY);
            });
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indSoldier1);
                return new StepGameUnit(game, isoMap, unit);
            });
    AddStep([]
            {
                const core::Pointd2D p0(1150, 150);
                return new StepGameConnectStructIntro(p0);
            });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const core::Pointd2D p0(1000, 250);
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const Cell2D target(23, 4);
                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indSoldier1);
                const core::Pointd2D p0(1200, 250);
                const Cell2D cellEnd(15, 5);
                return new StepGameConquerCellsEnd(game, isoMap, unit, cellEnd, p0);
            });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT SECOND ENERGY GENERATOR
    AddStep([game]
            {
                const int movX = 0;
                const int movY = 250;
                return new StepGameMoveCamera(movX, movY);
            });
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indWorker1);
                return new StepGameUnit(game, isoMap, unit);
            });
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
    AddStep([local, game, isoMap]
            {
                const auto unit = local->GetUnit(indSoldier1);
                return new StepGameUnit(game, isoMap, unit);
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
                return new StepGameUnit(game, isoMap, unit);
            });
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1100, 450);
                return new StepGameBuildStructIntro(panelActions, "TUT_GAME_BUILD_RES_CEN_1", p0);
            });
    AddStep([hud]
            {
                const int indCat = 3;
                const int indStruct = 0;
                return new StepGameBuildStructure(hud, "TUT_GAME_BUILD_RES_CEN_2", nullptr,
                                                  indCat, indStruct);
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
    // SETUP RESEARCH
    AddStep([panelActions]
            {
                const core::Pointd2D p0(1000, 650);
                return new StepGameSetupResearchIcon(panelActions, p0);
            });
}

TutorialGame2::~TutorialGame2()
{
    auto game = GetScreen()->GetGame();

    // clear perfect shot flag for unit soldier 1
    const Player * local = game->GetPlayerByIndex(0);
    auto unit1 = local->GetUnit(indSoldier1);

    if(unit1 != nullptr)
        unit1->SetPerfectShot(false);
}

} // namespace game
