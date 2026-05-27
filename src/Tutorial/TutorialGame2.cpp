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
#include "Tutorial/StepGameBaseBuildUnitStart.h"
#include "Tutorial/StepGameBaseBuildUnitEnd.h"
#include "Tutorial/StepGameConnectStructIntro.h"
#include "Tutorial/StepGameConquerCellsEnd.h"
#include "Tutorial/StepGameConquerCellsSimple.h"
#include "Tutorial/StepGameConquerStructChoice.h"
#include "Tutorial/StepGameDisableCamera.h"
#include "Tutorial/StepGameEndTurnSimple.h"
#include "Tutorial/StepGameSelectBase.h"
#include "Tutorial/StepGameSetSelectionDefaultAction.h"
#include "Tutorial/StepGameSetSelectionActiveAction.h"
#include "Tutorial/StepGameUnit.h"
#include "Tutorial/StepGameUnitConquerCellsIcon.h"
#include "Tutorial/StepGameWaitTurn.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/GameHUD.h"

namespace
{
using namespace game;

constexpr unsigned int indUnit1 = 0;

const Cell2D cellEneGen1(6, 15);
const Cell2D cellMatGen1(15, 7);

// data when conquering energy generator first
const Cell2D cellConquerEneGen1Start1(6, 13);
const Cell2D cellConquerEneGen1End1(6, 7);
const Cell2D cellConquerMatGen1Start1(7, 7);
const Cell2D cellConquerMatGen1End1(13, 7);
// data when conquering material generator first
const Cell2D cellConquerMatGen1Start2(13, 7);
const Cell2D cellConquerMatGen1End2(6, 7);
const Cell2D cellConquerEneGen1Start2(6, 8);
const Cell2D cellConquerEneGen1End2(6, 13);
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
    AddStep([localBase, game, isoMap] { return new StepGameSelectBase(game, isoMap, localBase); });
    AddStep([panelActions] { return new StepGameBaseBuildUnitStart(panelActions); });
    AddStep([hud] { return new StepGameBaseBuildUnitEnd(hud); });
    AddStep([this] { return new StepGameDisableCamera(GetCameraMapController()); });
    AddStep([localBase] { return new StepDelay(localBase->GetTimeBuildUnit()); });
    AddStep([local, game, isoMap]
        {
            const auto unit = local->GetUnit(indUnit1);
            return new StepGameUnit(game, isoMap, unit);
        });
    AddStep([] { return new StepDelay(0.5f); });
    // CONQUEST FIRST GENERATOR
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
                const core::Pointd2D p0(1000, 550);
                return new StepGameConnectStructIntro(p0);
            });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, isoMap, game]
            {
                const Cell2D & cellStart = GetOverlayCellConquest()->GetCellStart();
                const GameObject * gen1 = GetObjectInCell(cellEneGen1);
                const GameObject * gen2 = GetObjectInCell(cellMatGen1);

                core::Pointd2D p0(0, 400);
                Cell2D target;

                if(gen1->GetFaction() != NO_FACTION)
                {
                    mFirstGenConqueredIsEnergy = true;
                    target = cellConquerEneGen1Start1;
                    p0.x = 1100;
                }
                else
                {
                    mFirstGenConqueredIsEnergy = false;
                    p0.x = 400;
                    target = cellConquerMatGen1Start2;
                }

                return new StepGameConquerCellsSimple(game, isoMap, cellStart, target, p0);
            });
    AddStep([this, local, isoMap, game]
            {
                const auto unit = local->GetUnit(indUnit1);
                const core::Pointd2D p0(650, 250);

                Cell2D target;

                if(mFirstGenConqueredIsEnergy)
                    target = cellConquerEneGen1End1;
                else
                    target = cellConquerMatGen1End2;

                return new StepGameConquerCellsEnd(game, isoMap, unit, target, p0);
            });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::MOVE); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([gs] { return new StepGameWaitTurn(gs); });
    AddStep([] { return new StepDelay(0.5f); });
    // CONNECT AND CONQUER SECOND GENERATOR

}

} // namespace game
