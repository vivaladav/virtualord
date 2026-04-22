#include "Tutorial/TutorialGameIntro.h"

#include "CameraMapController.h"
#include "Game.h"
#include "GameMap.h"
#include "Player.h"
#include "GameObjects/Base.h"
#include "Indicators/OverlayCellConquest.h"
#include "Screens/ScreenGame.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepGameBase.h"
#include "Tutorial/StepGameBaseBuildUnit.h"
#include "Tutorial/StepGameBaseBuildUnitIcon.h"
#include "Tutorial/StepGameBaseFeatures.h"
#include "Tutorial/StepGameBuildTower.h"
#include "Tutorial/StepGameBuildTowerEnd.h"
#include "Tutorial/StepGameBuildTowerIntro.h"
#include "Tutorial/StepGameClearSelection.h"
#include "Tutorial/StepGameConnectStructIntro.h"
#include "Tutorial/StepGameConquerCells.h"
#include "Tutorial/StepGameConquerCellsEnd.h"
#include "Tutorial/StepGameConquerCellsSimple.h"
#include "Tutorial/StepGameConquerStruct.h"
#include "Tutorial/StepGameConquerStructSimple.h"
#include "Tutorial/StepGameDisableCamera.h"
#include "Tutorial/StepGameEnableCamera.h"
#include "Tutorial/StepGameEndTurn.h"
#include "Tutorial/StepGameEndTurnIntro.h"
#include "Tutorial/StepGameEndTurnSimple.h"
#include "Tutorial/StepGameEnergyRegeneration.h"
#include "Tutorial/StepGameIntro.h"
#include "Tutorial/StepGameMapNavigation.h"
#include "Tutorial/StepGameMaterialGenerator.h"
#include "Tutorial/StepGameMissionGoalsIcon.h"
#include "Tutorial/StepGameMissionGoalsDialog.h"
#include "Tutorial/StepGameMoveCamera.h"
#include "Tutorial/StepGameMoveUnit.h"
#include "Tutorial/StepGameMoveUnitToCorner.h"
#include "Tutorial/StepGameSetSelectionActiveAction.h"
#include "Tutorial/StepGameSetSelectionDefaultAction.h"
#include "Tutorial/StepGameStructConnected.h"
#include "Tutorial/StepGameStructDisconnected.h"
#include "Tutorial/StepGameTurnEnergy.h"
#include "Tutorial/StepGameUnit.h"
#include "Tutorial/StepGameUnitConquerCellsIcon.h"
#include "Tutorial/StepGameUpgradeIntro.h"
#include "Tutorial/StepGameUpgradeUnit.h"
#include "Tutorial/StepGameWaitTurn.h"
#include "Widgets/GameHUD.h"
#include "Tutorial/TutorialConstants.h"

#include <cassert>

namespace game
{

TutorialGameIntro::TutorialGameIntro(Screen * screen)
    : Tutorial(TUTORIAL_MISSION_INTRO)
    , mScreen(dynamic_cast<ScreenGame *>(screen))
{
    assert(mScreen);

    const Player * local = mScreen->GetGame()->GetLocalPlayer();

    auto panelActions = mScreen->mHUD->GetPanelObjectActions();
    auto panelObj = mScreen->mHUD->GetPanelSelectedObject();
    auto panelTurn = mScreen->mHUD->GetPanelTurnControl();
    auto localBase = local->GetBase();

    AddStep([this] { return new StepGameDisableCamera(mScreen->mCamController); });
    AddStep([] { return new StepDelay(1.f); });
    AddStep([] { return new StepGameIntro; });
    AddStep([] { return new StepDelay(0.3f); });
    AddStep([localBase] { return new StepGameBase(localBase); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelActions, panelObj] { return new StepGameBaseFeatures(panelObj, panelActions); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelActions] { return new StepGameBaseBuildUnitIcon(panelActions); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this] { return new StepGameBaseBuildUnit(mScreen->mHUD); });
    AddStep([this] { return new StepGameDisableCamera(mScreen->mCamController); });
    AddStep([localBase] { return new StepDelay(localBase->GetTimeBuildUnit()); });
    AddStep([local] { return new StepGameUnit(local); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local] { return new StepGameMoveUnit(local, mScreen->mIsoMap); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    // TODO update based on tutorial map
    AddStep([] { return new StepGameMoveCamera(450, -150); });
    AddStep([this, local]
            {
                const int genR = 31;
                const int genC = 11;
                const GameMapCell gmc = mScreen->mGameMap->GetCell(genR, genC);

                return new StepGameConquerStruct(local, gmc.objTop, mScreen->mIsoMap);
            });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::IDLE); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this] { return new StepGameTurnEnergy(mScreen->mHUD); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelTurn] { return new StepGameEndTurn(panelTurn); });
    AddStep([this] { return new StepGameWaitTurn(mScreen); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepDelay(1.0f); });
    AddStep([] { return new StepGameEnergyRegeneration; });
    AddStep([] { return new StepGameStructDisconnected; });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this]
        {
            const Cell2D & cellStart = mScreen->mOverlayCellConquest->GetCellStart();
            return new StepGameConquerCells(mScreen->mIsoMap, cellStart);
        });
    AddStep([this, local]
        {
            const Cell2D cellEnd(38, 10);
            return new StepGameConquerCellsEnd(mScreen->mIsoMap, local, cellEnd);
        });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([] { return new StepGameStructConnected; });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local]
        {
            const int genR = 22;
            const int genC = 10;
            const GameMapCell gmc = mScreen->mGameMap->GetCell(genR, genC);

            return new StepGameMaterialGenerator(gmc.objTop);
        });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::MOVE); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([this] { return new StepGameWaitTurn(mScreen); });
    AddStep([] { return new StepDelay(1.0f); });
    AddStep([] { return new StepGameMoveCamera(300, -150); });
    AddStep([this, local]
        {
            const int genR = 22;
            const int genC = 10;
            const GameMapCell gmc = mScreen->mGameMap->GetCell(genR, genC);

            return new StepGameConquerStructSimple(local, gmc.objTop, mScreen->mIsoMap);
        });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::IDLE); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepGameEndTurnIntro(); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([this] { return new StepGameWaitTurn(mScreen); });
    AddStep([] { return new StepDelay(1.0f); });
    AddStep([] { return new StepGameConnectStructIntro(); });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this]
        {
            const Cell2D & cellStart = mScreen->mOverlayCellConquest->GetCellStart();
            const Cell2D target(23, 10);
            return new StepGameConquerCellsSimple(mScreen->mIsoMap, cellStart, target);
        });
    AddStep([this, local]
        {
            const Cell2D cellEnd(29, 10);
            return new StepGameConquerCellsEnd(mScreen->mIsoMap, local, cellEnd);
        });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([] { return new StepDelay(1.0f); });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::MOVE); });
    AddStep([this, local] { return new StepGameMoveUnitToCorner(local, mScreen->mIsoMap); });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::IDLE); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([] { return new StepGameMoveCamera(400, -100); });
    AddStep([panelActions] { return new StepGameUpgradeIntro(panelActions); });
    AddStep([this] { return new StepGameUpgradeUnit(mScreen->mHUD); });
    AddStep([this] { return new StepGameDisableCamera(mScreen->mCamController); });
    AddStep([] { return new StepDelay(1.0f); });
    AddStep([panelTurn] { return new StepGameEndTurnSimple(panelTurn); });
    AddStep([] { return new StepDelay(1.0f); });
    AddStep([panelActions] { return new StepGameBuildTowerIntro(panelActions); });
    AddStep([this] { return new StepGameBuildTower(mScreen->mHUD); });
    AddStep([] { return new StepDelay(1.0f); });
    AddStep([this, local]
        {
            const Cell2D target(17, 16);
            return new StepGameBuildTowerEnd(mScreen->mIsoMap, local, target);
        });

    // TODO re-add mission goals
    //AddStep([panelActions] { return new StepGameMissionGoalsIcon(panelActions); });
    //AddStep([] { return new StepDelay(0.5f); });
    //AddStep([this] { return new StepGameMissionGoalsDialog(mScreen->mHUD); });
    //AddStep([this] { return new StepGameDisableCamera(mScreen->mCamController); });

    // TODO re-add map navigation
    // AddStep([this] { return new StepGameClearSelection(mScreen); });
    // AddStep([this] { return new StepGameEnableCamera(mScreen->mCamController); });
    // AddStep([] { return new StepGameMapNavigation; });
}

TutorialGameIntro::~TutorialGameIntro()
{
    // re-enable camera in game in case tutorial is quit
    mScreen->mCamController->SetEnabled(true);
}

} // namespace game
