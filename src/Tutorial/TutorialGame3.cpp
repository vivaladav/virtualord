#include "Tutorial/TutorialGame3.h"

#include "CameraMapController.h"
#include "Game.h"
#include "GameMap.h"
#include "Player.h"
#include "AI/PlayerAI.h"
#include "GameObjects/Base.h"
#include "GameObjects/Barracks.h"
#include "GameObjects/CityBlock.h"
#include "GameObjects/CityGroup.h"
#include "GameObjects/LootBox.h"
#include "GameObjects/MiniUnitsGroup.h"
#include "GameObjects/ObjectData.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "GameObjects/Unit.h"
#include "Screens/ScreenGame.h"
#include "Tutorial/StepAISetActive.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepDummy.h"
#include "Tutorial/StepGameAddEnemy.h"
#include "Tutorial/StepGameBackToBase.h"
#include "Tutorial/StepGameBuildStructAreaFree.h"
#include "Tutorial/StepGameBuildStructure.h"
#include "Tutorial/StepGameBuildUnit.h"
#include "Tutorial/StepGameBuildUnitEnd.h"
#include "Tutorial/StepGameBuildWallLine.h"
#include "Tutorial/StepGameCityIntro.h"
#include "Tutorial/StepGameClickActionButton.h"
#include "Tutorial/StepGameConnectStructFree.h"
#include "Tutorial/StepGameConnectWall.h"
#include "Tutorial/StepGameConquerCity.h"
#include "Tutorial/StepGameConquerStructFree.h"
#include "Tutorial/StepGameEndTurnSimple.h"
#include "Tutorial/StepGameEnemyIntro.h"
#include "Tutorial/StepGameEnemyKilled.h"
#include "Tutorial/StepGameEnding.h"
#include "Tutorial/StepGameIntro3.h"
#include "Tutorial/StepGameMissionGoalsIcon.h"
#include "Tutorial/StepGameMoveCameraOverCell.h"
#include "Tutorial/StepGameMoveCameraOverObject.h"
#include "Tutorial/StepGameMoveToCellFree.h"
#include "Tutorial/StepGameMoveUnitFree.h"
#include "Tutorial/StepGameMoveUnitSimple.h"
#include "Tutorial/StepGameOpenLootboxFree.h"
#include "Tutorial/StepGameQuickUnitButton.h"
#include "Tutorial/StepGameSecondaryMissionGoal.h"
#include "Tutorial/StepGameSelectObject.h"
#include "Tutorial/StepGameSetLootboxPrize.h"
#include "Tutorial/StepGameSetTarget.h"
#include "Tutorial/StepGameSetTargetIcon.h"
#include "Tutorial/StepGameSetupMiniUnits.h"
#include "Tutorial/StepGameSetupResearch.h"
#include "Tutorial/StepGameSetupResearchIcon.h"
#include "Tutorial/StepGameSingleInfo.h"
#include "Tutorial/StepGameSpawnIcon.h"
#include "Tutorial/StepGameTechTreeDialog.h"
#include "Tutorial/StepGameTechTreeIcon.h"
#include "Tutorial/StepGameUnit.h"
#include "Tutorial/StepGameUnitBuildIcon.h"
#include "Tutorial/StepGameWaitEnemyKilled.h"
#include "Tutorial/StepGameWaitObjectIdle.h"
#include "Tutorial/StepGameWaitTurn.h"
#include "Tutorial/StepGameWaitStructCanBuild.h"
#include "Tutorial/StepGameWaitUnitCanBuild.h"
#include "Tutorial/StepGameWaitUnitCanSpawn.h"
#include "Tutorial/StepSaveGame.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/GameHUD.h"
#include "Widgets/PanelObjectActions.h"

namespace
{
using namespace game;

const Cell2D cellTLCity1(51, 51);
const Cell2D cellBRCity1(54, 54);
const Cell2D cellLootbox1(74, 63);
const Cell2D cellSpecialLootbox1(61, 73);
const Cell2D cellMatGen1(66, 62);
const Cell2D cellMatGen2(41, 49);
const Cell2D cellMatGen3(51, 72);
const Cell2D cellEneGen1(62, 66);
const Cell2D cellEneGen2(49, 41);
const Cell2D cellEneGen3(73, 53);
const Cell2D cellEnemy1(64,53);
const Cell2D cellMiniUnits1(71, 62);

const Cell2D wallLeft1TL(50, 37);
const Cell2D wallLeft1BR(54, 37);
const Cell2D wallLeft2TL(57, 37);
const Cell2D wallLeft2BR(62, 37);
const Cell2D wallLeft3TL(71, 46);
const Cell2D wallLeft3BR(77, 46);
const Cell2D wallRight1TL(37, 50);
const Cell2D wallRight1BR(37, 54);
const Cell2D wallRight2TL(37, 57);
const Cell2D wallRight2BR(37, 63);
const Cell2D wallRight3TL(44, 72);
const Cell2D wallRight3BR(44, 77);

const Cell2D areaBarracksTL(64, 53);
const Cell2D areaBarracksBR(70, 59);
const Cell2D areaMatExtTL(72, 70);
const Cell2D areaMatExtBR(75, 78);
const Cell2D areaRadarTL(38, 38);
const Cell2D areaRadarBR(40, 40);
const Cell2D areaResearchTL(42, 42);
const Cell2D areaResearchBR(47, 47);
const Cell2D areaSolarTL(72, 72);
const Cell2D areaSolarBR(75, 80);
const Cell2D areaStorageTL(38, 38);
const Cell2D areaStorageBR(47, 47);

const unsigned int muSquads = 3;
const unsigned int muElements = 2;

constexpr TechUpgradeId upgrade1 = TECH_UP_RADAR_STATION;
constexpr TechUpgradeId upgrade2 = TECH_UP_STORAGE_STRUCTS;

}

namespace game
{

TutorialGame3::TutorialGame3(Screen * screen)
    : TutorialGame(screen, TUTORIAL_MISSION_3)
{
    using namespace sgl;

    auto gs = GetScreen();
    auto game = gs->GetGame();
    auto gameMap = GetGameMap();
    auto hud = gs->GetHUD();
    auto isoMap = GetIsoMap();

    const Player * local = game->GetActivePlayerByIndex(0);
    Player * playerAI = game->GetActivePlayerByIndex(1);

    auto panelActions = hud->GetPanelObjectActions();
    auto panelTurn = hud->GetPanelTurnControl();
    auto localBase = local->GetBase();

    // keep camera enabled in this tutorial
    GetCameraMapController()->SetEnabled(true);

    // ===== SETUP =====
    // pause before start
    AddStep([] { return new StepDelay(0.5f); });

    // ===== PART 1 =====
    // AUTO-SAVE
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    // INTRO
    AddStep([] { return new StepGameIntro3; });
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
                                                  TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([hud]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameBuildUnitEnd(hud, "TUT_GAME_BUILD_WORKER",
                                             TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([localBase] { return new StepDelay(localBase->GetTimeBuildUnit()); });
    // INTRO BUILD SOLAR PANELS AND MATERIAL EXTRACTOR
    AddStep([]
    {
        const int h = 175;
        const core::Pointd2D p0(1200, 250);
        return new StepGameSingleInfo(p0, "TUT_GAME_BUILD_RES_GEN", h);
    });
    // BUILD SOLAR PANELS
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_RES_GEN_ENERGY_SOLAR);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_SOL_PANELS",
                                                 isoMap, areaSolarTL, areaSolarBR);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameUnitBuildIcon(game, gs, panelActions, "TUT_GAME_BUILD_SOL_PANELS",
                                              ObjectData::TYPE_RES_GEN_ENERGY_SOLAR,
                                              TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local, hud]
    {
        const core::Pointd2D p0(1300, 100);
        const auto unit = static_cast<Unit *>(local->GetSelectedObject());
        auto step = new StepGameBuildStructure(unit, hud, "TUT_GAME_CAT_RES_GEN", nullptr,
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               ObjectData::TYPE_RES_GEN_ENERGY_SOLAR);
        step->SetDelayAnimation(2.f);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const int numToBuild = 2;

        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, areaSolarTL, areaSolarBR,
                                                    ObjectData::TYPE_RES_GEN_ENERGY_SOLAR,
                                                    numToBuild, "TUT_GAME_BUILD_SOL_PANELS");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    // CONNECT SECOND SOLAR PANEL IF NEEDED
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const auto all = local->GetStructuresByType(ObjectData::TYPE_RES_GEN_ENERGY_SOLAR);
        const GameObject * barracks = all.back();
        const core::Pointd2D p0(1200, 400);
        return new StepGameConnectStructFree(game, barracks, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // CONNECT FIRST SOLAR PANEL IF NEEDED
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const auto all = local->GetStructuresByType(ObjectData::TYPE_RES_GEN_ENERGY_SOLAR);
        const auto it = all.end() - 2;
        const GameObject * barracks = *it;
        const core::Pointd2D p0(1200, 400);
        return new StepGameConnectStructFree(game, barracks, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD MATERIAL EXTRACTOR
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 100);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_MAT_EXTR_1",
                                                 isoMap, areaMatExtTL, areaMatExtBR);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameUnitBuildIcon(game, gs, panelActions, "TUT_GAME_BUILD_MAT_EXTR_1",
                                              ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT,
                                              TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local, hud]
    {
        const core::Pointd2D p0(1300, 100);
        const auto unit = static_cast<Unit *>(local->GetSelectedObject());
        auto step = new StepGameBuildStructure(unit, hud, "TUT_GAME_CAT_RES_GEN",
                                               "TUT_GAME_BUILD_MAT_EXTR_2",
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT);
        step->SetDelayAnimation(2.f);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const int numToBuild = 1;

        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, areaMatExtTL, areaMatExtBR,
                                                    ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT,
                                                    numToBuild, "TUT_GAME_BUILD_MAT_EXTR_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    // CONNECT MATERIAL EXTRACTOR IF NEEDED
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const auto all = local->GetStructuresByType(ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT);
        const GameObject * barracks = all.back();
        const core::Pointd2D p0(1200, 400);
        return new StepGameConnectStructFree(game, barracks, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // INTRO MOVE WORKER TO LEFT SIDE
    AddStep([]
    {
        const core::Pointd2D p0(1300, 200);
        return new StepGameSingleInfo(p0, "TUT_GAME_MOVE_UNIT_3");
    });
    // MOVE CAMERA OVER TARGET
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([isoMap]
    {
        const Cell2D cell(74, 68);
        return new StepGameMoveCameraOverCell(cell, isoMap, TutorialConstants::speedCamera);
    });
    // MOVE WORKER TO LEFT SIDE
    AddStep([game, isoMap, local]
    {
        const core::Pointd2D p0(1300, 200);
        const Cell2D target(74, 66);
        const auto all = local->GetUnitsByType(ObjectData::TYPE_UNIT_WORKER1);
        const auto unit = all.back();
        return new StepGameMoveUnitFree(game, unit, isoMap, "TUT_GAME_MOVE_UNIT",
                                        target, p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // SETUP LOOTBOX
    AddStep([this]
    {
        auto lootbox = static_cast<LootBox *>(GetObjectInCell(cellLootbox1));
        const int prizeQuantity = 350;
        return new StepGameSetLootboxPrize(lootbox, ER_MATERIAL, prizeQuantity);
    });
    // OPEN LOOTBOX
    AddStep([this, gameMap, isoMap]
    {
        const core::Pointd2D p0(500, 100);
        auto lootbox = static_cast<LootBox *>(GetObjectInCell(cellLootbox1));
        return new StepGameOpenLootboxFree(gameMap, lootbox, isoMap, "TUT_GAME_LOOTBOX_1", p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONTINUE MOVE
    AddStep([]
    {
        const core::Pointd2D p0(1200, 250);
        return new StepGameSingleInfo(p0, "TUT_GAME_CONT_EXPL");
    });
    // MOVE WORKER TO MATERIAL GENERATOR
    AddStep([game, isoMap, local]
    {
        const core::Pointd2D p0(1300, 200);
        const Cell2D target(68, 62);
        const auto all = local->GetUnitsByType(ObjectData::TYPE_UNIT_WORKER1);
        const auto unit = all.back();
        return new StepGameMoveUnitFree(game, unit, isoMap, "TUT_GAME_MOVE_UNIT",
                                        target, p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER MATERIAL GENERATOR 1
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellMatGen1);
        const core::Pointd2D p0(1300, 300);
        return new StepGameConquerStructFree(game, gen, isoMap, "TUT_GAME_CONQUER_MAT_GEN", p0);
    });
    // MOVE VIEW BACK TO BASE
    AddStep([panelTurn, game]
        {
            const core::Pointd2D p0(50, 600);
            return new StepGameBackToBase(panelTurn, "TUT_GAME_BACK_TO_BASE_1", p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WORKER UNIT 2
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
                                                  TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([hud]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameBuildUnitEnd(hud, "TUT_GAME_BUILD_WORKER",
                                             TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([localBase] { return new StepDelay(localBase->GetTimeBuildUnit()); });
    // MOVE CAMERA
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game] { return new StepGameMoveCamera(0, -350); });
    // CONNECT MATERIAL GENERATOR 1
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellMatGen1);
        const core::Pointd2D p0(1300, 400);
        return new StepGameConnectStructFree(game, gen, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA NEAR ENERGY GENERATOR 1
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([isoMap]
    {
        const Cell2D cell(64, 65);
        return new StepGameMoveCameraOverCell(cell, isoMap, TutorialConstants::speedCamera);
    });
    // CONQUER ENERGY GENERATOR 1
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellEneGen1);
        const core::Pointd2D p0(1300, 200);
        return new StepGameConquerStructFree(game, gen, isoMap, "TUT_GAME_CONQUER_ENE_GEN", p0);
    });
    // CONNECT ENERGY GENERATOR 1
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellEneGen1);
        const core::Pointd2D p0(1300, 300);
        return new StepGameConnectStructFree(game, gen, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // SETUP SPECIAL LOOTBOX
    AddStep([this]
    {
        auto lootbox = static_cast<LootBox *>(GetObjectInCell(cellSpecialLootbox1));
        const int prizeQuantity = 0;
        return new StepGameSetLootboxPrize(lootbox, ER_NULL, prizeQuantity);
    });
    // OPEN SPECIAL LOOTBOX
    AddStep([this, gameMap, isoMap]
    {
        const core::Pointd2D p0(1000, 200);
        auto lootbox = static_cast<LootBox *>(GetObjectInCell(cellSpecialLootbox1));
        return new StepGameOpenLootboxFree(gameMap, lootbox, isoMap, "TUT_GAME_LOOTBOX_1b", p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([]
    {
        const core::Pointd2D p0(1150, 350);
        return new StepGameSingleInfo(p0, "TUT_GAME_NO_LUCK");
    });
    // ===== PART 2 =====
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // INTRO MOVE WORKER TO LEFT SIDE
    AddStep([]
    {
        const core::Pointd2D p0(1300, 200);
        return new StepGameSingleInfo(p0, "TUT_GAME_MOVE_UNIT_3");
    });
    // MOVE CAMERA OVER TARGET
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([isoMap]
    {
        const Cell2D cell(59, 59);
        return new StepGameMoveCameraOverCell(cell, isoMap, TutorialConstants::speedCamera);
    });
    // MOVE WORKER TO TOP
    AddStep([gameMap, isoMap, local]
    {
        const core::Pointd2D p0(1300, 200);
        const Cell2D target(56, 56);
        return new StepGameMoveToCellFree(gameMap, isoMap, "TUT_GAME_MOVE_UNIT_6",
                                          target, p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // INTRODUCE FIRST CITY AREA
    AddStep([isoMap]
    {
        const core::Pointd2D p0(1300, 150);
        return new StepGameCityIntro(cellTLCity1, cellBRCity1, isoMap, p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER CITY
    AddStep([this, local, isoMap, game]
    {
        const auto cb = static_cast<const CityBlock *>(GetObjectInCell(cellBRCity1));
        const auto cg = static_cast<const CityGroup *>(cb->GetGroup());
        const core::Pointd2D p0(1300, 200);
        return new StepGameConquerCity(cg, isoMap, "TUT_GAME_CITY_2", p0);
    });
    // CONNECT CITY
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellBRCity1);
        const core::Pointd2D p0(1300, 300);
        return new StepGameConnectStructFree(game, gen, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // INTRO MOVE WORKER TO LEFT SIDE
    AddStep([]
    {
        const core::Pointd2D p0(1300, 200);
        return new StepGameSingleInfo(p0, "TUT_GAME_CONT_EXPL");
    });
    // MOVE CAMERA OVER TARGET
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([isoMap]
    {
        const Cell2D cell(52, 52);
        return new StepGameMoveCameraOverCell(cell, isoMap, TutorialConstants::speedCamera);
    });
    // MOVE WORKER TO TOP
    AddStep([gameMap, isoMap, local]
    {
        const core::Pointd2D p0(1300, 200);
        const Cell2D target(47, 47);
        return new StepGameMoveToCellFree(gameMap, isoMap, "TUT_GAME_MOVE_UNIT_6",
                                          target, p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER MATERIAL GENERATOR 2
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellMatGen2);
        const core::Pointd2D p0(1300, 150);
        return new StepGameConquerStructFree(game, gen, isoMap, "TUT_GAME_CONQUER_MAT_GEN", p0);
    });
    // CONNECT MATERIAL GENERATOR 2
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellMatGen2);
        const core::Pointd2D p0(1300, 500);
        return new StepGameConnectStructFree(game, gen, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER ENERGY GENERATOR 2
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellEneGen2);
        const core::Pointd2D p0(1300, 200);
        return new StepGameConquerStructFree(game, gen, isoMap, "TUT_GAME_CONQUER_ENE_GEN", p0);
    });
    // CONNECT ENERGY GENERATOR 2
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellEneGen2);
        const core::Pointd2D p0(1300, 200);
        return new StepGameConnectStructFree(game, gen, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD RESEARCH CENTER
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 100);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_RESEARCH_CENTER);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_RES_CEN_1",
                                                 isoMap, areaResearchTL, areaResearchBR);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameUnitBuildIcon(game, gs, panelActions, "TUT_GAME_BUILD_RES_CEN_1",
                                              ObjectData::TYPE_RESEARCH_CENTER,
                                              TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local, hud]
    {
        const core::Pointd2D p0(1300, 100);
        const auto unit = static_cast<Unit *>(local->GetSelectedObject());
        auto step = new StepGameBuildStructure(unit, hud, "TUT_GAME_BUILD_RES_CEN_2", nullptr,
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               ObjectData::TYPE_RESEARCH_CENTER);
        step->SetDelayAnimation(TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const int numToBuild = 1;

        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, areaResearchTL, areaResearchBR,
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
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT RESEARCH CENTER
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const auto all = local->GetStructuresByType(ObjectData::TYPE_RESEARCH_CENTER);
        const GameObject * rc = all.back();

        return new StepGameSelectObject(game, isoMap, rc, "TUT_GAME_RES_CEN_1", p0,
                                        TutorialConstants::delayAnimLong);
    });
    // SETUP RESEARCH
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1000, 650);
        return new StepGameSetupResearchIcon(game, gs, panelActions, p0);
    });
    AddStep([hud]
    {
        return new StepGameSetupResearch(hud, { 100, 80, 70 });
    });
    // SAVE GAME
    AddStep([this]
    {
        const GameObject * obj = GetObjectInCell(cellEneGen2);
        const float speed = 800.f;
        return new StepGameMoveCameraOverObject(obj, speed);
    });
    // ===== PART 3 =====
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL LEFT 1
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1200, 200);
        auto step = new StepGameBuildWallLine(gameMap, isoMap, wallLeft1TL, wallLeft1BR,
                                              "TUT_GAME_BUILD_WALL_5");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    // CONNECT WALL LEFT 1
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 200);
        auto step = new StepGameConnectWall(gameMap, isoMap, wallLeft1TL, wallLeft1BR,
                                            "TUT_GAME_CONNECT_WALL");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD BUNKER IN LEFT WALL 1
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 600);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_BUNKER);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_BUNKER_1",
                                                 isoMap, wallLeft1TL, wallLeft1BR);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameUnitBuildIcon(game, gs, panelActions, "TUT_GAME_BUILD_BUNKER_1",
                                              ObjectData::TYPE_BUNKER,
                                              TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local, hud]
    {
        const core::Pointd2D p0(1300, 100);
        const auto unit = static_cast<Unit *>(local->GetSelectedObject());
        auto step = new StepGameBuildStructure(unit, hud, "TUT_GAME_BUILD_DTOWER_3", nullptr,
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               ObjectData::TYPE_BUNKER);
        step->SetDelayAnimation(TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const unsigned int numToBuild = 1;
        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, wallLeft1TL, wallLeft1BR,
                                                    ObjectData::TYPE_BUNKER, numToBuild,
                                                    "TUT_GAME_BUILD_BUNKER_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL LEFT 2
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1200, 200);
        auto step = new StepGameBuildWallLine(gameMap, isoMap, wallLeft2TL, wallLeft2BR,
                                              "TUT_GAME_BUILD_WALL_5");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    // CONNECT WALL LEFT 2
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 200);
        auto step = new StepGameConnectWall(gameMap, isoMap, wallLeft2TL, wallLeft2BR,
                                            "TUT_GAME_CONNECT_WALL");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
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
    AddStep([game, local]
    {
        TutorialStep * step = nullptr;

        if(local->IsUpgradeUnlocked(upgrade1) || !game->CanLocalPlayerAffordTechUpgrade(upgrade1))
            step = new StepDummy;
        else
        {
            const core::Pointd2D p0(685, 200);
            step = new StepGameSingleInfo(p0, "TUT_GAME_RES_CEN_2");
        }

        return step;
    });
    AddStep([game, isoMap, local]
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
    AddStep([this, game, gs, panelActions, local]
    {
        TutorialStep * step = nullptr;

        if(local->IsUpgradeUnlocked(upgrade1))
            step = new StepDummy;
        else
        {
            const core::Pointd2D p0(1000, 650);
            step = new StepGameTechTreeIcon(game, gs, panelActions, p0);

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
    AddStep([game, hud, local]
    {
        TutorialStep * step = nullptr;

        if(local->IsUpgradeUnlocked(upgrade1))
            step = new StepDummy;
        else
            step = new StepGameTechTreeDialog(game, hud, upgrade1, false);

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
            step = new StepGameMoveCameraOverCell(wallLeft2TL, isoMap,
                                                  TutorialConstants::speedCamera);

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
    // BUILD GATE IN LEFT WALL 2
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 100);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_WALL_GATE);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_GATE_1",
                                                 isoMap, wallLeft2TL, wallLeft2BR);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameUnitBuildIcon(game, gs, panelActions, "TUT_GAME_BUILD_GATE_1",
                                              ObjectData::TYPE_WALL_GATE,
                                              TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local, hud]
    {
        const core::Pointd2D p0(1300, 100);
        const auto unit = static_cast<Unit *>(local->GetSelectedObject());
        auto step = new StepGameBuildStructure(unit, hud, "TUT_GAME_BUILD_DTOWER_3",
                                               "TUT_GAME_BUILD_GATE_2",
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               ObjectData::TYPE_WALL_GATE);
        step->SetDelayAnimation(2.f);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const unsigned int numToBuild = 1;
        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, wallLeft2TL, wallLeft2BR,
                                                    ObjectData::TYPE_WALL_GATE, numToBuild,
                                                    "TUT_GAME_BUILD_GATE_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([] { return new StepDelay(0.5f); });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD TOWER IN LEFT WALL 2
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 600);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_DEFENSIVE_TOWER);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_DTOWER_1",
                                                 isoMap, wallLeft2TL, wallLeft2BR);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameUnitBuildIcon(game, gs, panelActions, "TUT_GAME_BUILD_DTOWER_1",
                                              ObjectData::TYPE_DEFENSIVE_TOWER,
                                              TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local, hud]
    {
        const core::Pointd2D p0(1300, 100);
        const auto unit = static_cast<Unit *>(local->GetSelectedObject());
        auto step = new StepGameBuildStructure(unit, hud, "TUT_GAME_BUILD_DTOWER_3",
                                               "TUT_GAME_BUILD_DTOWER_4",
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               ObjectData::TYPE_DEFENSIVE_TOWER);
        step->SetDelayAnimation(TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const unsigned int numToBuild = 1;
        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, wallLeft2TL, wallLeft2BR,
                                                    ObjectData::TYPE_DEFENSIVE_TOWER, numToBuild,
                                                    "TUT_GAME_BUILD_DTOWER_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    // ===== PART 4 =====
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
    AddStep([local, game]
    {
        const core::Pointd2D p0(1300, 100);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_UNIT_SPAWNER1);

        auto step = new StepGameWaitStructCanBuild(local, data, ObjectData::TYPE_BASE,
                                                   "TUT_GAME_BUILD_SPAWNER_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameClickActionButton(game, gs, panelActions, "TUT_GAME_BUILD_SPAWNER_1",
                                                  PanelObjectActions::BTN_BUILD_UNIT_BASE,
                                                  TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([hud]
    {
        return new StepGameBuildUnit(hud, "TUT_GAME_BUILD_SPAWNER_2", "TUT_GAME_BASE_BUILD_UNIT_3",
                                     TutorialConstants::unitMiniSpawner);
    });
    AddStep([localBase] { return new StepDelay(localBase->GetTimeBuildUnit()); });
    // SELECT SPAWNER
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local, game, isoMap]
    {
        const auto all = local->GetUnitsByType(ObjectData::TYPE_UNIT_SPAWNER1);
        const auto unit = all.back();
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
    AddStep([isoMap]
    {
        const Cell2D target(73, 67);
        return new StepGameMoveCameraOverCell(target, isoMap, TutorialConstants::speedCamera);
    });
    // MOVE SPAWNER NEAR ENEMY
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([local, isoMap, game]
    {
        const auto all = local->GetUnitsByType(ObjectData::TYPE_UNIT_SPAWNER1);
        const auto unit = all.back();
        const Cell2D target(70, 63);
        const core::Pointd2D p0(1100, 300);
        return new StepGameMoveUnitSimple(game, unit, isoMap, target, p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // SPAWN MINI UNITS
    AddStep([local, game]
    {
        const core::Pointd2D p0(1300, 100);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_MINI_UNIT1);

        auto step = new StepGameWaitUnitCanSpawn(local, data, muSquads, muElements,
                                                 "TUT_GAME_BUILD_MU");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
        {
            const core::Pointd2D p0(200, 600);
            return new StepGameSpawnIcon(game, gs, panelActions, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([hud]
    {
        return new StepGameSetupMiniUnits(hud, muSquads , muElements, ObjectData::TYPE_MINI_UNIT1);
    });
    AddStep([local]
    {
        const auto all = local->GetUnitsByType(ObjectData::TYPE_UNIT_SPAWNER1);
        const auto unit = all.back();
        return new StepGameWaitObjectIdle(unit);
    });
    // SELECT MINI UNITS
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, game, isoMap]
    {
        const auto mu = GetObjectInCell(cellMiniUnits1);
        const core::Pointd2D p0(500, 250);
        return new StepGameSelectObject(game, isoMap, mu, "TUT_GAME_MU_1", p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // SET TARGET FOR MINI UNITS
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(200, 600);
        return new StepGameSetTargetIcon(game, gs, panelActions, p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, isoMap]
    {
        const auto mu = GetObjectInCell(cellMiniUnits1);
        const auto group = static_cast<MiniUnitsGroup *>(mu->GetGroup());
        const Cell2D target(65,52);
        const core::Pointd2D p0(900, 150);
        return new StepGameSetTarget(isoMap, group, target, p0);
    });
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    // WAIT ENEMY KILL
    AddStep([this]
    {
        return new StepGameWaitEnemyKilled(GetObjectInCell(cellEnemy1), GetGameMap());
    });
    AddStep([this] { return new StepGameEnemyKilled(); });
    // ===== PART 5 =====
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA TO TOP
    AddStep([isoMap]
    {
        const Cell2D cell(48, 48);
        return new StepGameMoveCameraOverCell(cell, isoMap, TutorialConstants::speedCamera);
    });
    // BUILD WALL RIGHT 1
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 150);
        auto step = new StepGameBuildWallLine(gameMap, isoMap, wallRight1TL, wallRight1BR,
                                              "TUT_GAME_BUILD_WALL_5");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    // CONNECT WALL RIGHT 1
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 500);
        auto step = new StepGameConnectWall(gameMap, isoMap, wallRight1TL, wallRight1BR,
                                            "TUT_GAME_CONNECT_WALL");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD BUNKER IN RIGHT WALL 1
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 600);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_BUNKER);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_BUNKER_1",
                                                 isoMap, wallRight1TL, wallRight1BR);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameUnitBuildIcon(game, gs, panelActions, "TUT_GAME_BUILD_BUNKER_1",
                                              ObjectData::TYPE_BUNKER,
                                              TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local, hud]
    {
        const core::Pointd2D p0(1300, 100);
        const auto unit = static_cast<Unit *>(local->GetSelectedObject());
        auto step = new StepGameBuildStructure(unit, hud, "TUT_GAME_BUILD_DTOWER_3", nullptr,
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               ObjectData::TYPE_BUNKER);
        step->SetDelayAnimation(TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const unsigned int numToBuild = 1;
        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, wallRight1TL, wallRight1BR,
                                                    ObjectData::TYPE_BUNKER, numToBuild,
                                                    "TUT_GAME_BUILD_BUNKER_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL RIGHT 2
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1200, 200);
        auto step = new StepGameBuildWallLine(gameMap, isoMap, wallRight2TL, wallRight2BR,
                                              "TUT_GAME_BUILD_WALL_5");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    // CONNECT WALL RIGHT 2
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 200);
        auto step = new StepGameConnectWall(gameMap, isoMap, wallRight2TL, wallRight2BR,
                                            "TUT_GAME_CONNECT_WALL");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD GATE IN RIGHT WALL 2
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 100);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_WALL_GATE);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_GATE_1",
                                                 isoMap, wallRight2TL, wallRight2BR);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameUnitBuildIcon(game, gs, panelActions, "TUT_GAME_BUILD_GATE_1",
                                              ObjectData::TYPE_WALL_GATE,
                                              TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local, hud]
    {
        const core::Pointd2D p0(1300, 100);
        const auto unit = static_cast<Unit *>(local->GetSelectedObject());
        auto step = new StepGameBuildStructure(unit, hud, "TUT_GAME_BUILD_DTOWER_3",
                                               "TUT_GAME_BUILD_GATE_2",
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               ObjectData::TYPE_WALL_GATE);
        step->SetDelayAnimation(2.f);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const unsigned int numToBuild = 1;
        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, wallRight2TL, wallRight2BR,
                                                    ObjectData::TYPE_WALL_GATE, numToBuild,
                                                    "TUT_GAME_BUILD_GATE_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([] { return new StepDelay(0.5f); });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD TOWER IN RIGHT WALL 2
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 600);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_DEFENSIVE_TOWER);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_DTOWER_1",
                                                 isoMap, wallRight2TL, wallRight2BR);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameUnitBuildIcon(game, gs, panelActions, "TUT_GAME_BUILD_DTOWER_1",
                                              ObjectData::TYPE_DEFENSIVE_TOWER,
                                              TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local, hud]
    {
        const core::Pointd2D p0(1300, 100);
        const auto unit = static_cast<Unit *>(local->GetSelectedObject());
        auto step = new StepGameBuildStructure(unit, hud, "TUT_GAME_BUILD_DTOWER_3",
                                               "TUT_GAME_BUILD_DTOWER_4",
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               ObjectData::TYPE_DEFENSIVE_TOWER);
        step->SetDelayAnimation(TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const unsigned int numToBuild = 1;
        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, wallRight2TL, wallRight2BR,
                                                    ObjectData::TYPE_DEFENSIVE_TOWER, numToBuild,
                                                    "TUT_GAME_BUILD_DTOWER_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // INTRO MOVE WORKER TO MATERIAL GENERATOR 3
    AddStep([]
    {
        const core::Pointd2D p0(1300, 200);
        return new StepGameSingleInfo(p0, "TUT_GAME_CONT_EXPL");
    });
    // MOVE CAMERA OVER TARGET
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([isoMap]
    {
        const Cell2D cell(41, 69);
        return new StepGameMoveCameraOverCell(cell, isoMap, TutorialConstants::speedCamera);
    });
    // MOVE WORKER MATERIAL GENERATOR 3
    AddStep([gameMap, isoMap, local]
    {
        const core::Pointd2D p0(1300, 200);
        const Cell2D target(45, 72);
        return new StepGameMoveToCellFree(gameMap, isoMap, "TUT_GAME_MOVE_UNIT_6",
                                          target, p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER MATERIAL GENERATOR 3
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellMatGen3);
        const core::Pointd2D p0(1300, 150);
        return new StepGameConquerStructFree(game, gen, isoMap, "TUT_GAME_CONQUER_MAT_GEN", p0);
    });
    // CONNECT MATERIAL GENERATOR 3
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellMatGen3);
        const core::Pointd2D p0(1300, 150);
        return new StepGameConnectStructFree(game, gen, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA OVER TARGET
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([isoMap]
    {
        const Cell2D cell(49, 69);
        return new StepGameMoveCameraOverCell(cell, isoMap, TutorialConstants::speedCamera);
    });
    // BUILD WALL RIGHT 3
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1200, 200);
        auto step = new StepGameBuildWallLine(gameMap, isoMap, wallRight3TL, wallRight3BR,
                                              "TUT_GAME_BUILD_WALL_5");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    // CONNECT WALL RIGHT 3
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 200);
        auto step = new StepGameConnectWall(gameMap, isoMap, wallRight3TL, wallRight3BR,
                                            "TUT_GAME_CONNECT_WALL");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD GATE IN RIGHT WALL 3
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 100);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_WALL_GATE);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_GATE_1",
                                                 isoMap, wallRight3TL, wallRight3BR);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameUnitBuildIcon(game, gs, panelActions, "TUT_GAME_BUILD_GATE_1",
                                              ObjectData::TYPE_WALL_GATE,
                                              TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local, hud]
    {
        const core::Pointd2D p0(1300, 100);
        const auto unit = static_cast<Unit *>(local->GetSelectedObject());
        auto step = new StepGameBuildStructure(unit, hud, "TUT_GAME_BUILD_DTOWER_3",
                                               "TUT_GAME_BUILD_GATE_2",
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               ObjectData::TYPE_WALL_GATE);
        step->SetDelayAnimation(2.f);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const unsigned int numToBuild = 1;
        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, wallRight3TL, wallRight3BR,
                                                    ObjectData::TYPE_WALL_GATE, numToBuild,
                                                    "TUT_GAME_BUILD_GATE_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD BUNKER IN RIGHT WALL 3
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 600);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_BUNKER);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_BUNKER_1",
                                                 isoMap, wallRight3TL, wallRight3BR);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameUnitBuildIcon(game, gs, panelActions, "TUT_GAME_BUILD_BUNKER_1",
                                              ObjectData::TYPE_BUNKER,
                                              TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local, hud]
    {
        const core::Pointd2D p0(1300, 100);
        const auto unit = static_cast<Unit *>(local->GetSelectedObject());
        auto step = new StepGameBuildStructure(unit, hud, "TUT_GAME_BUILD_DTOWER_3", nullptr,
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               ObjectData::TYPE_BUNKER);
        step->SetDelayAnimation(TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const unsigned int numToBuild = 1;
        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, wallRight3TL, wallRight3BR,
                                                    ObjectData::TYPE_BUNKER, numToBuild,
                                                    "TUT_GAME_BUILD_BUNKER_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    // ===== PART 6 =====
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // INTRO MOVE WORKER TO ENERGY GENERATOR 3
    AddStep([]
    {
        const core::Pointd2D p0(1300, 200);
        return new StepGameSingleInfo(p0, "TUT_GAME_CONT_EXPL");
    });
    // MOVE CAMERA OVER TARGET
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([isoMap]
    {
        const Cell2D cell(65, 60);
        return new StepGameMoveCameraOverCell(cell, isoMap, TutorialConstants::speedCamera);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE WORKER ENERGY GENERATOR 3
    AddStep([gameMap, isoMap, local]
    {
        const core::Pointd2D p0(1300, 200);
        const Cell2D target(70, 57);
        return new StepGameMoveToCellFree(gameMap, isoMap, "TUT_GAME_MOVE_UNIT_6",
                                          target, p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUER ENERGY GENERATOR 3
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellEneGen3);
        const core::Pointd2D p0(1300, 150);
        return new StepGameConquerStructFree(game, gen, isoMap, "TUT_GAME_CONQUER_MAT_GEN", p0);
    });
    // CONNECT ENERGY GENERATOR 3
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const GameObject * gen = GetObjectInCell(cellEneGen3);
        const core::Pointd2D p0(1300, 150);
        return new StepGameConnectStructFree(game, gen, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA OVER TARGET
    AddStep([isoMap]
    {
        const Cell2D cell(71, 51);
        return new StepGameMoveCameraOverCell(cell, isoMap, TutorialConstants::speedCamera);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL LEFT 3
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1200, 200);
        auto step = new StepGameBuildWallLine(gameMap, isoMap, wallLeft3TL, wallLeft3BR,
                                              "TUT_GAME_BUILD_WALL_5");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    // CONNECT WALL LEFT 3
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 200);
        auto step = new StepGameConnectWall(gameMap, isoMap, wallLeft3TL, wallLeft3BR,
                                            "TUT_GAME_CONNECT_WALL");
        step->SetInfoPosition(p0.x, p0.y);
        return step;
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD GATE IN LEFT WALL 3
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 100);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_WALL_GATE);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_GATE_1",
                                                 isoMap, wallLeft3TL, wallLeft3BR);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameUnitBuildIcon(game, gs, panelActions, "TUT_GAME_BUILD_GATE_1",
                                              ObjectData::TYPE_WALL_GATE,
                                              TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local, hud]
    {
        const core::Pointd2D p0(1300, 100);
        const auto unit = static_cast<Unit *>(local->GetSelectedObject());
        auto step = new StepGameBuildStructure(unit, hud, "TUT_GAME_BUILD_DTOWER_3",
                                               "TUT_GAME_BUILD_GATE_2",
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               ObjectData::TYPE_WALL_GATE);
        step->SetDelayAnimation(2.f);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const unsigned int numToBuild = 1;
        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, wallLeft3TL, wallLeft3BR,
                                                    ObjectData::TYPE_WALL_GATE, numToBuild,
                                                    "TUT_GAME_BUILD_GATE_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD BUNKER IN LEFT WALL 3
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 600);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_BUNKER);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_BUNKER_1",
                                                 isoMap, wallLeft3TL, wallLeft3BR);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameUnitBuildIcon(game, gs, panelActions, "TUT_GAME_BUILD_BUNKER_1",
                                              ObjectData::TYPE_BUNKER,
                                              TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local, hud]
    {
        const core::Pointd2D p0(1300, 100);
        const auto unit = static_cast<Unit *>(local->GetSelectedObject());
        auto step = new StepGameBuildStructure(unit, hud, "TUT_GAME_BUILD_DTOWER_3", nullptr,
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               ObjectData::TYPE_BUNKER);
        step->SetDelayAnimation(TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const unsigned int numToBuild = 1;
        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, wallLeft3TL, wallLeft3BR,
                                                    ObjectData::TYPE_BUNKER, numToBuild,
                                                    "TUT_GAME_BUILD_BUNKER_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE VIEW BACK TO BASE
    AddStep([panelTurn]
    {
        const core::Pointd2D p0(100, 500);
        return new StepGameBackToBase(panelTurn, "TUT_GAME_BACK_TO_BASE_1b", p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // COLLECT SECONDARY MISSION GOAL
    AddStep([localBase, game, isoMap]
    {
        const core::Pointd2D p0(500, 200);
        return new StepGameSelectObject(game, isoMap, localBase, "TUT_GAME_BASE_4", p0);
    });
    AddStep([game, gs, panelActions]
    {
        return new StepGameMissionGoalsIcon(game, gs, panelActions, false);
    });
    AddStep([hud]
    {
        const int goal = 0;
        return new StepGameSecondaryMissionGoal(hud, goal);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA OVER TARGET
    AddStep([isoMap]
    {
        const Cell2D cell(70, 54);
        return new StepGameMoveCameraOverCell(cell, isoMap, TutorialConstants::speedCamera);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD BARRACKS
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 100);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_BARRACKS);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_BARRACKS_1",
                                                 isoMap, areaBarracksTL, areaBarracksBR);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);

        auto step = new StepGameUnitBuildIcon(game, gs, panelActions, "TUT_GAME_BUILD_BARRACKS_1",
                                              ObjectData::TYPE_BARRACKS,
                                              TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local, hud]
    {
        const core::Pointd2D p0(1300, 100);
        const auto unit = static_cast<Unit *>(local->GetSelectedObject());
        auto step = new StepGameBuildStructure(unit, hud, nullptr, nullptr,
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               ObjectData::TYPE_BARRACKS);
        step->SetDelayAnimation(TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const int numToBuild = 1;

        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, areaBarracksTL, areaBarracksBR,
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
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
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
                                                  TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([hud]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameBuildUnitEnd(hud, "TUT_GAME_BUILD_LSOLDIER",
                                             TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local]
    {
        const auto all = local->GetStructuresByType(ObjectData::TYPE_BARRACKS);
        const auto barracks = static_cast<Barracks *>(all.back());
        return new StepDelay(barracks->GetTimeBuildUnit());
    });
    // ===== PART 7 =====
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA OVER TARGET
    AddStep([isoMap]
    {
        const Cell2D cell(44, 44);
        return new StepGameMoveCameraOverCell(cell, isoMap, TutorialConstants::speedCamera);
    });
    AddStep([]
    {
        const core::Pointd2D p0(1300, 200);
        return new StepGameSingleInfo(p0, "TUT_GAME_BUILD_RADAR_1");
    });
    // MOVE WORKER TO TOP
    AddStep([gameMap, isoMap, local]
    {
        const core::Pointd2D p0(1300, 200);
        const Cell2D target(48, 48);
        return new StepGameMoveToCellFree(gameMap, isoMap, "TUT_GAME_MOVE_UNIT_6",
                                          target, p0);
    });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD RADAR STATION
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 100);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_RADAR_STATION);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_RADAR_1",
                                                 isoMap, areaRadarTL, areaRadarBR);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);

        auto step = new StepGameUnitBuildIcon(game, gs, panelActions, "TUT_GAME_BUILD_RADAR_1",
                                              ObjectData::TYPE_RADAR_STATION,
                                              TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local, hud]
    {
        const core::Pointd2D p0(1300, 100);
        const auto unit = static_cast<Unit *>(local->GetSelectedObject());
        auto step = new StepGameBuildStructure(unit, hud, "TUT_GAME_BUILD_RES_CEN_2",
                                               "TUT_GAME_BUILD_RADAR_2",
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               ObjectData::TYPE_RADAR_STATION);
        step->SetDelayAnimation(TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const int numToBuild = 1;

        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, areaRadarTL, areaRadarBR,
                                                    ObjectData::TYPE_RADAR_STATION, numToBuild,
                                                    "TUT_GAME_BUILD_RADAR_1");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    // CONNECT RADAR STATION IF NEEDED
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const auto all = local->GetStructuresByType(ObjectData::TYPE_RADAR_STATION);
        const GameObject * radar = all.back();
        const core::Pointd2D p0(1200, 400);
        return new StepGameConnectStructFree(game, radar, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // MOVE CAMERA OVER RESEARCH CENTER
    AddStep([this, local]
    {
        TutorialStep * step = nullptr;

        if(local->IsUpgradeUnlocked(upgrade2))
            step = new StepDummy;
        else
        {
            const auto all = local->GetStructuresByType(ObjectData::TYPE_RESEARCH_CENTER);
            const GameObject * rc = all.back();
            const float speed = 1000.f;
            step = new StepGameMoveCameraOverObject(rc, speed);
        }

        return step;
    });
    AddStep([local]
    {
        TutorialStep * step = nullptr;

        if(local->IsUpgradeUnlocked(upgrade2))
            step = new StepDummy;
        else
            step = new StepDelay(0.5f);

        return step;
    });
    AddStep([local]
    {
        TutorialStep * step = nullptr;

        if(local->IsUpgradeUnlocked(upgrade2))
            step = new StepDummy;
        else
        {
            const core::Pointd2D p0(685, 200);
            step = new StepGameSingleInfo(p0, "TUT_GAME_RES_CEN_2");
        }

        return step;
    });
    // SELECT RESEARCH CENTER
    AddStep([game, isoMap, local]
    {
        TutorialStep * step = nullptr;

        if(local->IsUpgradeUnlocked(upgrade2))
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
    // UNLOCK TECH UPGRADE 2
    AddStep([this, game, gs, panelActions, local]
    {
        TutorialStep * step = nullptr;

        if(local->IsUpgradeUnlocked(upgrade2))
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

        if(local->IsUpgradeUnlocked(upgrade2))
            step = new StepDummy;
        else
            step = new StepDelay(0.5f);

        return step;
    });
    AddStep([game, hud, local]
    {
        TutorialStep * step = nullptr;

        if(local->IsUpgradeUnlocked(upgrade2))
            step = new StepDummy;
        else
            step = new StepGameTechTreeDialog(game, hud, upgrade2, false);

        return step;
    });
    // ENABLE AI
    AddStep([playerAI] { return new StepAISetActive(playerAI->GetAI(), true); });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD MATERIAL STORAGE
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 600);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_RES_STORAGE_MATERIAL);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_STORAGE_3",
                                                 isoMap, areaStorageTL, areaStorageBR);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);

        auto step = new StepGameUnitBuildIcon(game, gs, panelActions, "TUT_GAME_BUILD_STORAGE_3",
                                              ObjectData::TYPE_RES_STORAGE_MATERIAL,
                                              TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local, hud]
    {
        const core::Pointd2D p0(1300, 100);
        const auto unit = static_cast<Unit *>(local->GetSelectedObject());
        auto step = new StepGameBuildStructure(unit, hud, "TUT_GAME_CAT_RES_GEN",
                                               "TUT_GAME_BUILD_STORAGE_1",
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               ObjectData::TYPE_RES_STORAGE_MATERIAL);
        step->SetDelayAnimation(TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const int numToBuild = 1;
        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, areaStorageTL, areaStorageBR,
                                                    ObjectData::TYPE_RES_STORAGE_MATERIAL,
                                                    numToBuild, "TUT_GAME_BUILD_STORAGE_3");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    // CONNECT MATERIAL STORAGE IF NEEDED
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const auto all = local->GetStructuresByType(ObjectData::TYPE_RES_STORAGE_MATERIAL);
        const GameObject * st = all.back();
        const core::Pointd2D p0(1200, 400);
        return new StepGameConnectStructFree(game, st, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs, true); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD ENERGY STORAGE
    AddStep([local, game, isoMap]
    {
        const core::Pointd2D p0(1300, 600);

        const auto reg = game->GetObjectsRegistry();
        const ObjectData & data = reg->GetObjectData(ObjectData::TYPE_RES_STORAGE_ENERGY);

        auto step = new StepGameWaitUnitCanBuild(local, data, "TUT_GAME_BUILD_STORAGE_4",
                                                 isoMap, areaStorageTL, areaStorageBR);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([game, gs, panelActions]
    {
        const core::Pointd2D p0(1300, 100);
        auto step = new StepGameUnitBuildIcon(game, gs, panelActions, "TUT_GAME_BUILD_STORAGE_4",
                                              ObjectData::TYPE_RES_STORAGE_ENERGY,
                                              TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([local, hud]
    {
        const core::Pointd2D p0(1300, 100);
        const auto unit = static_cast<Unit *>(local->GetSelectedObject());
        auto step = new StepGameBuildStructure(unit, hud, "TUT_GAME_CAT_RES_GEN",
                                               "TUT_GAME_BUILD_STORAGE_2",
                                               "TUT_GAME_BASE_BUILD_UNIT_3",
                                               ObjectData::TYPE_RES_STORAGE_ENERGY);
        step->SetDelayAnimation(TutorialConstants::delayAnimLong);
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    AddStep([gameMap, isoMap]
    {
        const core::Pointd2D p0(1300, 100);
        const int numToBuild = 1;
        auto step = new StepGameBuildStructAreaFree(gameMap, isoMap, areaStorageTL, areaStorageBR,
                                                    ObjectData::TYPE_RES_STORAGE_ENERGY,
                                                    numToBuild, "TUT_GAME_BUILD_STORAGE_4");
        step->SetInfoPosition(p0.x, p0.y);

        return step;
    });
    // CONNECT ENERGY STORAGE IF NEEDED
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local, isoMap, game]
    {
        const auto all = local->GetStructuresByType(ObjectData::TYPE_RES_STORAGE_ENERGY);
        const GameObject * st = all.back();
        const core::Pointd2D p0(1200, 400);
        return new StepGameConnectStructFree(game, st, isoMap, "TUT_GAME_CONNECT_STRUCT", p0);
    });
    // FINAL MESSAGE
    AddStep([game]
    {
        const core::Pointd2D p0(635, 200);
        return new StepGameEnding(game, p0);
    });
    // FINAL SAVE GAME
    AddStep([game, gs] { return new StepSaveGame(game, gs); });
    AddStep([] { return new StepDelay(0.5f); });
}

} // namespace game
