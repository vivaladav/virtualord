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
#include "Tutorial/StepGameBackToBase.h"
#include "Tutorial/StepGameBuildStructIntro.h"
#include "Tutorial/StepGameBuildStructure.h"
#include "Tutorial/StepGameBuildTowerEnd.h"
#include "Tutorial/StepGameBuildUnitEnd.h"
#include "Tutorial/StepGameBuildUnitStart.h"
#include "Tutorial/StepGameWallBuildIntro.h"
#include "Tutorial/StepGameCityIntro.h"
#include "Tutorial/StepGameConnectStructIntro.h"
#include "Tutorial/StepGameConquerCellsEnd.h"
#include "Tutorial/StepGameConquerCellsSimple.h"
#include "Tutorial/StepGameConquerEnergyGenIntro.h"
#include "Tutorial/StepGameConquerMaterialGenIntro.h"
#include "Tutorial/StepGameConquerStructSimple.h"
#include "Tutorial/StepGameDisableCamera.h"
#include "Tutorial/StepGameEndTurnSimple.h"
#include "Tutorial/StepGameIntro3.h"
#include "Tutorial/StepGameMoveCamera.h"
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

const Cell2D cellTLCity1(51, 51);
const Cell2D cellBRCity1(54, 54);
const Cell2D cellCityBlock2(54, 52);
const Cell2D cellCityBlock3(52, 52);
const Cell2D cellCityBlock4(52, 54);
const Cell2D cellLootbox1(74, 63);
const Cell2D cellResCenter(45, 47);
const Cell2D cellSpecialLootbox1(61, 73);
const Cell2D cellMatGen1(66, 62);
const Cell2D cellMatGen2(41, 49);
const Cell2D cellEneGen1(62, 66);
const Cell2D cellEneGen2(49, 41);
const Cell2D cellTower1(58, 58);
const Cell2D cellTower2(48, 48);
const Cell2D cellBunker1(37, 50);
const Cell2D cellBunker2(50, 37);

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
    AddStep([game, panelActions]
        {
            return new StepGameBuildUnitStart(game, panelActions, PanelObjectActions::BTN_BUILD_UNIT_BASE);
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
    AddStep([game, panelActions]
        {
           const core::Pointd2D p0(1100, 300);
            return new StepGameBuildStructIntro(game, panelActions, "TUT_GAME_BUILD_RES_GEN", p0);
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
    AddStep([game, panelActions]
        {
            const core::Pointd2D p0(1100, 250);
            return new StepGameBuildStructIntro(game, panelActions, "TUT_GAME_BUILD_MORE", p0);
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
    AddStep([game, panelActions] { return new StepGameUnitConquerCellsIcon(game, panelActions); });
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
    AddStep([game, panelActions]
        {
            const core::Pointd2D p0(1100, 250);
            return new StepGameBuildStructIntro(game, panelActions, "TUT_GAME_BUILD_MORE", p0);
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
    AddStep([game, panelActions] { return new StepGameUnitConquerCellsIcon(game, panelActions); });
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
            const int prizeQuantity = 200;
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
    AddStep([game, panelActions]
        {
            return new StepGameBuildUnitStart(game, panelActions, PanelObjectActions::BTN_BUILD_UNIT_BASE);
        });
    AddStep([hud] { return new StepGameBuildUnitEnd(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([localBase] { return new StepDelay(localBase->GetTimeBuildUnit()); });
    // SELECT WORKER 2
    AddStep([] { return new StepDelay(0.5f); });
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
    AddStep([game, panelActions] { return new StepGameUnitConquerCellsIcon(game, panelActions); });
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
    AddStep([game, panelActions] { return new StepGameUnitConquerCellsIcon(game, panelActions); });
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
    AddStep([game, panelActions] { return new StepGameUnitConquerCellsIcon(game, panelActions); });
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
    AddStep([game, panelActions] { return new StepGameUnitConquerCellsIcon(game, panelActions); });
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
    AddStep([panelActions]
        {
            core::Pointd2D p0(900, 150);
            return new StepGameUpgradeIntro(panelActions, "TUT_GAME_UPGRADE_1b", p0);
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
    AddStep([game, panelActions]
            {
                const core::Pointd2D p0(1000, 250);
                return new StepGameBuildStructIntro(game, panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
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
    AddStep([game, panelActions] { return new StepGameUnitConquerCellsIcon(game, panelActions); });
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
    AddStep([game, panelActions] { return new StepGameUnitConquerCellsIcon(game, panelActions); });
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
    AddStep([game, panelActions] { return new StepGameUnitConquerCellsIcon(game, panelActions); });
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
    AddStep([panelActions]
        {
            core::Pointd2D p0(900, 150);
            return new StepGameUpgradeIntro(panelActions, "TUT_GAME_UPGRADE_1b", p0);
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
    AddStep([game, panelActions]
            {
                const core::Pointd2D p0(1000, 250);
                return new StepGameBuildStructIntro(game, panelActions, "TUT_GAME_BUILD_DTOWER_1", p0);
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
    AddStep([game, panelActions] { return new StepGameUnitConquerCellsIcon(game, panelActions); });
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
    AddStep([game, panelActions] { return new StepGameUnitConquerCellsIcon(game, panelActions); });
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
    AddStep([game, panelActions] { return new StepGameUnitConquerCellsIcon(game, panelActions); });
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
    AddStep([panelActions, game]
            {
                core::Pointd2D p0(1000, 300);
                return new StepGameUpgradeIntro(panelActions, "TUT_GAME_UPGRADE_1b", p0);
            });
    AddStep([hud] { return new StepGameUpgradeUnitFree(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
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
    AddStep([game, panelActions]
        {
            const core::Pointd2D p0(1100, 500);
            return new StepGameBuildStructIntro(game, panelActions, "TUT_GAME_BUILD_RES_CEN_1", p0);
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
    AddStep([] { return new StepDelay(0.5f); });
    // SETUP RESEARCH
    AddStep([this, game, isoMap]
        {
            const core::Pointd2D p0(1100, 600);
            const GameObject * obj = GetObjectInCell(cellResCenter);
            return new StepGameSelectObject(game, isoMap, obj, "TUT_GAME_RES_CEN_1", p0);
        });
    AddStep([game, panelActions]
        {
            const core::Pointd2D p0(1000, 650);
            return new StepGameSetupResearchIcon(game, panelActions, p0);
        });
    AddStep([hud] { return new StepGameSetupResearch(hud, { 100, 70, 70 }); });
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 2
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indWorker2);
            const core::Pointd2D p0(1100, 350);
            return new StepGameUnit(game, isoMap, unit, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD BUNKER WITH WORKER 2
    AddStep([game, panelActions]
        {
            const core::Pointd2D p0(1000, 250);
            return new StepGameBuildStructIntro(game, panelActions, "TUT_GAME_BUILD_BUNKER_1", p0);
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
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT BUNKER WITH WORKER 2
    AddStep([game, panelActions] { return new StepGameUnitConquerCellsIcon(game, panelActions); });
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
    AddStep([game, panelActions]
        {
            const core::Pointd2D p0(1000, 250);
            return new StepGameBuildStructIntro(game, panelActions, "TUT_GAME_BUILD_BUNKER_1", p0);
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
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT BUNKER WITH WORKER 1
    AddStep([game, panelActions] { return new StepGameUnitConquerCellsIcon(game, panelActions); });
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
    // END TURN
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL CELL WITH WORKER 1
    AddStep([] { return new StepGameWallBuildIntro(); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game, panelActions] { return new StepGameWallBuildIcon(game, panelActions); });
    AddStep([isoMap, local, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const Cell2D cellEnd(50, 38);
            const core::Pointd2D p0(900, 300);
            return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD GATE WITH WORKER 1
    AddStep([game, panelActions]
        {
            const core::Pointd2D p0(1100, 400);
            return new StepGameBuildStructIntro(game, panelActions, "TUT_GAME_BUILD_GATE_1", p0);
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
            const Cell2D target(51, 38);
            return new StepGameBuildTowerEnd(isoMap, unit, target, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL CELL WITH WORKER 1
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game, panelActions] { return new StepGameWallBuildIcon(game, panelActions); });
    AddStep([isoMap, local, game]
        {
            const auto unit = local->GetUnit(indWorker1);
            const Cell2D cellEnd(52, 38);
            const core::Pointd2D p0(900, 300);
            return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // SELECT WORKER 2
    AddStep([hud]
        {
            const sgl::core::Pointd2D p0(100, 600);
            return new StepGameQuickUnitButton(hud, indWorker2, nullptr, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL CELL WITH WORKER 2
    AddStep([] { return new StepGameWallBuildIntro(); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game, panelActions] { return new StepGameWallBuildIcon(game, panelActions); });
    AddStep([isoMap, local, game]
        {
            const auto unit = local->GetUnit(indWorker2);
            const Cell2D cellEnd(38, 50);
            const core::Pointd2D p0(900, 300);
            return new StepGameWallBuildEnd(game, isoMap, unit, cellEnd, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD GATE WITH WORKER 1
    AddStep([game, panelActions]
        {
            const core::Pointd2D p0(1100, 400);
            return new StepGameBuildStructIntro(game, panelActions, "TUT_GAME_BUILD_GATE_1", p0);
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
            const Cell2D target(38, 51);
            return new StepGameBuildTowerEnd(isoMap, unit, target, p0);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // BUILD WALL CELL WITH WORKER 2
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([game, panelActions] { return new StepGameWallBuildIcon(game, panelActions); });
    AddStep([isoMap, local, game]
        {
            const auto unit = local->GetUnit(indWorker2);
            const Cell2D cellEnd(38, 52);
            const core::Pointd2D p0(900, 300);
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
}

TutorialGame3::~TutorialGame3()
{

}

} // namespace game
