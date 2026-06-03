#include "Tutorial/TutorialGame2.h"

#include "CameraMapController.h"
#include "Game.h"
#include "Player.h"
#include "GameObjects/Base.h"
#include "Indicators/OverlayCellConquest.h"
#include "Screens/ScreenGame.h"
#include "Tutorial/StepAISetActive.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepGameIntro2.h"
#include "Tutorial/StepGameBuildStructIntro.h"
#include "Tutorial/StepGameBuildTowerEnd.h"
#include "Tutorial/StepGameBuildUnitEnd.h"
#include "Tutorial/StepGameBuildUnitStart.h"
#include "Tutorial/StepGameConnectStructIntro.h"
#include "Tutorial/StepGameConquerCellsEnd.h"
#include "Tutorial/StepGameConquerCellsSimple.h"
#include "Tutorial/StepGameConquerGeneratorIntro.h"
#include "Tutorial/StepGameConquerStructChoice.h"
#include "Tutorial/StepGameConquerStructSimple.h"
#include "Tutorial/StepGameDisableCamera.h"
#include "Tutorial/StepGameEndTurnSimple.h"
#include "Tutorial/StepGameMoveUnitSimple.h"
#include "Tutorial/StepGameSelectObject.h"
#include "Tutorial/StepGameSetSelectionDefaultAction.h"
#include "Tutorial/StepGameSetSelectionActiveAction.h"
#include "Tutorial/StepGameUnit.h"
#include "Tutorial/StepGameUnitConquerCellsIcon.h"
#include "Tutorial/StepGameWaitTurn.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/GameHUD.h"
#include "Widgets/PanelObjectActions.h"

namespace
{
using namespace game;

constexpr unsigned int indWorker1 = 0;
constexpr unsigned int indSoldier1 = 1;

const Cell2D cellEneGen1(6, 15);
const Cell2D cellMatGen1(15, 7);
const Cell2D cellBarracks(8, 9);

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
    AddStep([this, local, isoMap]
        {
            const auto unit = local->GetUnit(indSoldier1);
            const Cell2D target(15, 11);
            const core::Pointd2D p0(1100, 650);
            return new StepGameMoveUnitSimple(unit, isoMap, target, p0);
        });
    // ATTACK TREES WITH SOLDIER

}

} // namespace game
