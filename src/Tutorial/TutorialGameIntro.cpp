#include "Tutorial/TutorialGameIntro.h"

#include "CameraMapController.h"
#include "Game.h"
#include "GameMap.h"
#include "Player.h"
#include "GameObjects/Base.h"
#include "Screens/ScreenGame.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepGameBase.h"
#include "Tutorial/StepGameBaseBuildUnit.h"
#include "Tutorial/StepGameBaseBuildUnitIcon.h"
#include "Tutorial/StepGameBaseFeatures.h"
#include "Tutorial/StepGameClearSelection.h"
#include "Tutorial/StepGameConquerCells.h"
#include "Tutorial/StepGameConquerStruct.h"
#include "Tutorial/StepGameDisableCamera.h"
#include "Tutorial/StepGameEnableCamera.h"
#include "Tutorial/StepGameEndTurn.h"
#include "Tutorial/StepGameEnergyRegeneration.h"
#include "Tutorial/StepGameIntro.h"
#include "Tutorial/StepGameMapNavigation.h"
#include "Tutorial/StepGameMissionGoalsIcon.h"
#include "Tutorial/StepGameMissionGoalsDialog.h"
#include "Tutorial/StepGameMoveCamera.h"
#include "Tutorial/StepGameMoveUnit.h"
#include "Tutorial/StepGameSetSelectionActiveAction.h"
#include "Tutorial/StepGameSetSelectionDefaultAction.h"
#include "Tutorial/StepGameStructConnected.h"
#include "Tutorial/StepGameStructDisconnected.h"
#include "Tutorial/StepGameTurnEnergy.h"
#include "Tutorial/StepGameUnit.h"
#include "Tutorial/StepGameUnitConquerCellsIcon.h"
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
    AddStep([panelActions] { return new StepGameMissionGoalsIcon(panelActions); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this] { return new StepGameMissionGoalsDialog(mScreen->mHUD); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([panelActions] { return new StepGameBaseBuildUnitIcon(panelActions); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this] { return new StepGameBaseBuildUnit(mScreen->mHUD); });
    AddStep([localBase] { return new StepDelay(localBase->GetTimeBuildUnit()); });
    AddStep([local] { return new StepGameUnit(local); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([this, local] { return new StepGameMoveUnit(local, mScreen->mIsoMap); });
    AddStep([local] { return new StepGameSetSelectionActiveAction(local, GameObjectActionType::IDLE); });
    // TODO update based on tutorial map
    AddStep([] { return new StepGameMoveCamera(200, -100); });
    AddStep([this, local]
            {
                const int genR = 56;
                const int genC = 13;
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
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([] { return new StepGameEnergyRegeneration; });
    AddStep([] { return new StepGameStructDisconnected; });
    AddStep([panelActions] { return new StepGameUnitConquerCellsIcon(panelActions); });
    AddStep([this, local] { return new StepGameConquerCells(local, mScreen->mIsoMap); });
    AddStep([local] { return new StepGameSetSelectionDefaultAction(local, GameObjectActionType::MOVE); });
    AddStep([this] { return new StepGameClearSelection(mScreen); });
    AddStep([] { return new StepDelay(0.5f); });
    AddStep([] { return new StepGameStructConnected; });

    AddStep([this] { return new StepGameEnableCamera(mScreen->mCamController); });

    AddStep([] { return new StepGameMapNavigation; });
}

TutorialGameIntro::~TutorialGameIntro()
{
    // re-enable camera in game in case tutorial is quit
    mScreen->mCamController->SetEnabled(true);
}

} // namespace game
