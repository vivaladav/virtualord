#include "GameHUD.h"

#include "Cell2D.h"
#include "ControlMap.h"
#include "Game.h"
#include "GameConstants.h"
#include "GameMap.h"
#include "GameUIData.h"
#include "IsoMap.h"
#include "IsoObject.h"
#include "Player.h"
#include "GameObjects/GameObject.h"
#include "GameObjects/Structure.h"
#include "GameObjects/Temple.h"
#include "GameObjects/Unit.h"
#include "GameObjectTools/Weapon.h"
#include "Screens/ScreenGame.h"
#include "Tutorial/TutorialManager.h"
#include "Widgets/ButtonMinimap.h"
#include "Widgets/ButtonPanelSelectedObject.h"
#include "Widgets/ButtonQuickUnitSelection.h"
#include "Widgets/CountdownLabel.h"
#include "Widgets/DialogEndMission.h"
#include "Widgets/DialogExit.h"
#include "Widgets/DialogExploreTemple.h"
#include "Widgets/DialogMissionGoals.h"
#include "Widgets/DialogNewElement.h"
#include "Widgets/DialogNewMiniUnitsSquad.h"
#include "Widgets/DialogObject.h"
#include "Widgets/DialogResearch.h"
#include "Widgets/DialogTrading.h"
#include "Widgets/DialogUpgrade.h"
#include "Widgets/GameMapProgressBar.h"
#include "Widgets/MiniMap.h"
#include "Widgets/PanelHit.h"
#include "Widgets/PanelObjectActions.h"
#include "Widgets/PanelResources.h"
#include "Widgets/PanelSelectedObject.h"
#include "Widgets/PanelSelfDestruction.h"
#include "Widgets/PanelShotType.h"
#include "Widgets/PanelTurnControl.h"

#include <sgl/graphic/Camera.h>
#include <sgl/graphic/Renderer.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/ButtonsGroup.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Stage.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

GameHUD::GameHUD(ScreenGame * screen)
    : mScreen(screen)
{
    using namespace sgl;

    auto sm = utilities::StringManager::Instance();
    sm->AddListener(this);

    const int rendW = graphic::Renderer::Instance()->GetWidth();
    const int rendH = graphic::Renderer::Instance()->GetHeight();

    SetSize(rendW, rendH);

    Game * game = mScreen->GetGame();

    // LOCAL PLAYER
    Player * player = game->GetLocalPlayer();

    // react to local player changes in stats
    player->SetOnResourcesChanged([this]
    {
        if(mDialogNewElement != nullptr)
            mDialogNewElement->CheckBuild();
    });

    // TOP RESOURCE BAR
    mPanelRes = new PanelResources(player, mScreen->mGameMap, this);
    mPanelRes->SetX((rendW - mPanelRes->GetWidth()) / 2);

    // MINIMAP
    mButtonMinimap = new ButtonMinimap(this);
    mButtonMinimap->SetX(rendW - mButtonMinimap->GetWidth());

    mButtonMinimap->AddOnClickFunction([this]
    {
        OpenPanelMinimap();
    });

    mMiniMap = new MiniMap(mScreen->mCamController, mScreen->mIsoMap, this);
    mMiniMap->SetVisible(false);
    mMiniMap->SetX(rendW - mMiniMap->GetWidth());

    mMiniMap->AddFunctionOnClose([this]
    {
        ClosePanelMinimap();
    });

    // QUICK UNIT SELECTION BUTTONS
    mGroupUnitSel = new sgl::sgui::ButtonsGroup(sgl::sgui::ButtonsGroup::HORIZONTAL, this);

    const int numButtons = player->GetMaxUnits();

    for(int i = 0; i < numButtons; ++i)
    {
        auto b = new ButtonQuickUnitSelection(i, screen);
        mGroupUnitSel->AddButton(b);
    }

    const int groupX = (rendW - mGroupUnitSel->GetWidth()) * 0.5f;
    const int groupY = rendH - mGroupUnitSel->GetHeight();
    mGroupUnitSel->SetPosition(groupX, groupY);

    player->SetOnNumUnitsChanged([this, player]
    {
        const int numUnits = player->GetNumUnits();
        const int maxUnits = player->GetMaxUnits();

        for(int i = 0; i < numUnits; ++i)
        {
            auto b = static_cast<ButtonQuickUnitSelection *>(mGroupUnitSel->GetButton(i));
            b->SetUnit(player->GetUnit(i));
        }

        for(int i = numUnits; i < maxUnits; ++i)
        {
            auto b = static_cast<ButtonQuickUnitSelection *>(mGroupUnitSel->GetButton(i));
            b->ClearUnit();
        }
    });

    // OBJECT ACTIONS
    mPanelObjActions = new PanelObjectActions(this);
    mPanelObjActions->SetVisible(false);

    // PANEL TURN CONTROL
    mPanelTurnCtrl = new PanelTurnControl(player, this);
    mPanelTurnCtrl->SetFunctionEndTurn([this]
    {
        mScreen->EndTurn();
    });

    const int posPanelTurnX = (rendW - mPanelTurnCtrl->GetWidth()) / 2;
    const int posPanelTurnY = groupY - mPanelTurnCtrl->GetHeight();
    mPanelTurnCtrl->SetPosition(posPanelTurnX, posPanelTurnY);

    // PANEL SELECTED OBJECT
    const ObjectsDataRegistry * odr = game->GetObjectsRegistry();

    mPanelSelObj = new PanelSelectedObject(odr, this);
    mPanelSelObj->SetVisible(false);

    mPanelSelObj->AddFunctionOnClose([this]
    {
        ClosePanelSelectedObject();
    });

    mPanelSelObj->AddFunctionOnShowInfo([this]
    {
        Player * player = mScreen->GetGame()->GetLocalPlayer();

        ShowDialogObject(player->GetSelectedObject());
    });

    mPanelSelObj->AddFunctionOnShowUpgrade([this]
    {
        ShowDialogUpgrade(mPanelSelObj->GetObject());
    });

    mButtonPanelSelObj = new ButtonPanelSelectedObject(this);
    mButtonPanelSelObj->SetVisible(false);

    mButtonPanelSelObj->AddOnClickFunction([this]
    {
        OpenPanelSelectedObject();
    });

    // PANEL HIT
    mPanelHit = new PanelHit;
    mPanelHit->SetVisible(false);
}

GameHUD::~GameHUD()
{
    Player * player = mScreen->GetGame()->GetLocalPlayer();

    player->SetOnNumUnitsChanged([]{});
    player->SetOnResourcesChanged([]{});
}

void GameHUD::SetMiniMapEnabled(bool val)
{
    if(mMiniMap->IsEnabled() == val)
        return ;

    mMiniMap->SetEnabled(val);
    mMiniMap->SetVisible(val);

    mButtonMinimap->SetEnabled(val);
    mButtonMinimap->SetVisible(false);
}

void GameHUD::HidePanelObjectActions()
{
    mPanelObjActions->ClearObject();
    mPanelObjActions->SetVisible(false);
}

void GameHUD::ShowPanelObjectActions(GameObject * obj)
{
    mPanelObjActions->SetObject(obj);
    mPanelObjActions->SetVisible(true);
    mPanelObjActions->SetActionsEnabled(obj->GetCurrentAction() == IDLE);
}

void GameHUD::HidePanelSelfDestruction()
{
    if(mPanelSelfDestruct == nullptr)
        return ;

    // schedule dialog deletion
    mPanelSelfDestruct->DeleteLater();
    mPanelSelfDestruct = nullptr;
}

void GameHUD::ShowPanelSelfDestruction()
{
    if(mPanelSelfDestruct != nullptr)
        return ;

    // CREATE DIALOG
    mPanelSelfDestruct = new PanelSelfDestruction;

    sgl::sgui::Stage::Instance()->SetFocus();

    // button Destroy
    mPanelSelfDestruct->AddFunctionOnDestroy([this]
                                             {
                                                 auto obj = mScreen->mLocalPlayer->GetSelectedObject();

                                                 mScreen->mGameMap->RemoveAndDestroyObject(obj);

                                                 HidePanelSelfDestruction();
                                             });

    // button Blow Up
    mPanelSelfDestruct->AddFunctionOnBlowup([this]
                                            {
                                                auto obj = mScreen->mLocalPlayer->GetSelectedObject();
                                                obj->SelfDestroy();

                                                HidePanelSelfDestruction();
                                            });

    // position dialog
    PositionOptionsPanelOverObjectActions(mPanelSelfDestruct, PanelObjectActions::BTN_SELF_DESTROY);
}

void GameHUD::HidePanelShotType()
{
    if(mPanelShotType == nullptr)
        return ;

    // schedule dialog deletion
    mPanelShotType->DeleteLater();
    mPanelShotType = nullptr;
}

void GameHUD::ShowPanelShotType()
{
    if(mPanelShotType != nullptr)
        return ;

    // CREATE PANEL
    auto selObj = mScreen->mLocalPlayer->GetSelectedObject();
    const unsigned int am = selObj->GetWeapon()->GetAttackMode();

    mPanelShotType = new PanelShotType;
    mPanelShotType->SetButtonChecked(am, true);

    sgl::sgui::Stage::Instance()->SetFocus();

    // change Attack Mode
    mPanelShotType->SetFunctionOnToggle([this, selObj](unsigned int ind, bool checked)
    {
        if(!checked)
            return ;

        selObj->SetAttackMode(static_cast<AttackMode>(ind));

        // update panel hit if currently visible
        if(mPanelHit->IsVisible())
            mScreen->UpdatePanelHit(selObj);
    });

    // position dialog
    PositionOptionsPanelOverObjectActions(mPanelShotType, PanelObjectActions::BTN_ATTACK);
}

void GameHUD::ShowPanelHit(const GameObject * attacker, const GameObject * target)
{
    using namespace sgl;

    mPanelHit->SetVisible(true);
    mPanelHit->ShowAttackerData(attacker, target);

    // POSITION PANEL
    const int rendW = graphic::Renderer::Instance()->GetWidth();
    const int rendH = graphic::Renderer::Instance()->GetHeight();

    auto camera = graphic::Camera::GetDefaultCamera();

    const int panelW = mPanelHit->GetWidth();
    const int panelH = mPanelHit->GetHeight();

    const IsoObject * isoTarget = target->GetIsoObject();
    const int isoX = camera->GetWorldToScreenX(isoTarget->GetX());
    const int isoY = camera->GetWorldToScreenY(isoTarget->GetY());

    int posX = isoX + isoTarget->GetWidth();
    int posY = isoY + (isoTarget->GetHeight() - panelH) / 2;

    if((posX + panelW) > rendW)
        posX = isoX - panelW;

    if(posY < 0)
        posY = 0;

    mPanelHit->SetPosition(posX, posY);
}

void GameHUD::HidePanelHit()
{
    mPanelHit->SetVisible(false);
}

void GameHUD::SetQuickUnitButtonChecked(GameObject * obj)
{
    // check corresponding quick unit selection button
    const int numButtons = mGroupUnitSel->GetNumButtons();

    for(int i = 0; i < numButtons; ++i)
    {
        auto b = static_cast<ButtonQuickUnitSelection *>(mGroupUnitSel->GetButton(i));
        Unit * unit = b->GetUnit();

        if(unit == obj)
        {
            b->SetChecked(true);
            break;
        }
    }
}

void GameHUD::ClearQuickUnitButtonChecked()
{
    const int checked = mGroupUnitSel->GetIndexChecked();

    if(checked != -1)
        mGroupUnitSel->GetButton(checked)->SetChecked(false);
}

void GameHUD::ShowDialogMissionGoals()
{
    if(mDialogMissionGoals != nullptr)
        return ;

    mScreen->ShowScreenOverlay();

    ++mVisibleDialogs;

    mScreen->SetPause(true);

    Game * game = mScreen->GetGame();
    mDialogMissionGoals = new DialogMissionGoals(mScreen);
    mDialogMissionGoals->SetFocus();

    mDialogMissionGoals->AddFunctionOnClose([this]
    {
        HideDialogMissionGoals();

        ReopenPanels();

        // un-pause game
        mScreen->SetPause(false);
    });

    mDialogMissionGoals->AddFunctionOnEnd([this]
    {
        HideDialogMissionGoals();

        // show dialog game won
        ShowDialogEndMission(true);
    });

    TemporaryClosePanels();

    // position dialog
    CenterWidget(mDialogMissionGoals);
}

void GameHUD::HideDialogMissionGoals()
{
    if(mDialogMissionGoals == nullptr)
        return ;

    mScreen->HideScreenOverlay();

    --mVisibleDialogs;

    // schedule dialog deletion
    mDialogMissionGoals->DeleteLater();
    mDialogMissionGoals = nullptr;
}

void GameHUD::ShowDialogEndMission(bool won)
{
    mScreen->SetPause(true);

    mScreen->ShowScreenOverlay();

    ++mVisibleDialogs;

    // stats
    GameMap * gm = mScreen->mGameMap;

    const PlayerFaction pf = mScreen->GetGame()->GetLocalPlayerFaction();
    const int territory = gm->GetControlMap()->GetPercentageControlledByFaction(pf);
    const unsigned int killed = gm->GetEnemiesKilled(pf);
    const unsigned int casualties = gm->GetCasualties(pf);
    const unsigned int played = mScreen->GetPlayTimeInSec();

    // create dialog
    mDialogEnd = new DialogEndMission(played, territory, killed, pf, won);
    mDialogEnd->SetFocus();

    mDialogEnd->SetFunctionOnClose([this, won]
    {
        mScreen->HideScreenOverlay();

        --mVisibleDialogs;

        mDialogEnd->DeleteLater();
        mDialogEnd = nullptr;

        if(won)
            mScreen->HandleGameWon();
        else
            mScreen->HandleGameOver();
    });

    TemporaryClosePanels();

    // position dialog
    CenterWidget(mDialogEnd);
}

void GameHUD::ShowDialogExit()
{
    if(mDialogExit != nullptr)
        return ;

    // special cases, do not open exit dialog if temple outcome is on screen
    // as dialog explore temple outcome can't be closed if no option is selected
    // OR if showing dialog exit as mission is over
    if(mDialogExploreTempleOutcome != nullptr || mDialogEnd != nullptr)
        return ;

    mScreen->ShowScreenOverlay();

    ++mVisibleDialogs;

    mScreen->SetPause(true);

    auto tutMan = mScreen->GetGame()->GetTutorialManager();

    auto buttons = DialogExit::BUTTONS_EXIT;

    if(tutMan->HasActiveTutorial())
    {
        tutMan->SetTutorialPause(true);

        buttons = static_cast<DialogExit::DialogButtons>(DialogExit::BTN_MAIN_MENU |
                                                         DialogExit::BUTTONS_TUTORIAL);
    }

    mDialogExit = new DialogExit(buttons, mScreen->GetGame(), mScreen);

    mDialogExit->SetFocus();

    mDialogExit->SetFunctionOnShowingDialogSettings([this]
    {
        ++mVisibleDialogs;

        TemporaryClosePanels();

        // keep game paused
        mScreen->SetPause(true);
    });

    mDialogExit->SetFunctionOnHidingDialogSettings([this]
    {
        --mVisibleDialogs;

        ReopenPanels();

        ResumeGameFromExit();
    });

    mDialogExit->SetFunctionOnClose([this]
    {
        mScreen->HideScreenOverlay();

        --mVisibleDialogs;

        // schedule dialog deletion
        mDialogExit->DeleteLater();
        mDialogExit = nullptr;

        if(0 ==mVisibleDialogs)
            ReopenPanels();

        ResumeGameFromExit();
    });

    TemporaryClosePanels();

    // position dialog
    CenterWidget(mDialogExit);
}

void GameHUD::ShowDialogExploreTemple(Player * player, Temple * temple)
{
    if(mDialogExploreTemple != nullptr)
        return ;

    mScreen->ShowScreenOverlay();

    ++mVisibleDialogs;

    mScreen->SetPause(true);

    mDialogExploreTemple = new DialogExploreTemple(player, temple);

    mDialogExploreTemple->SetFocus();

    mDialogExploreTemple->SetFunctionOnCancel([this]
    {
        HideDialogExploreTemple();
    });

    mDialogExploreTemple->SetFunctionOnExplore([this, player, temple]
    {
        HideDialogExploreTemple();

        // spend allocated resources
        player->SumResource(Player::Stat::MONEY, -temple->GetInvestedMoney());
        player->SumResource(Player::Stat::MATERIAL, -temple->GetInvestedMaterial());
        player->SumResource(Player::Stat::BLOBS, -temple->GetInvestedBlobs());
        player->SumResource(Player::Stat::DIAMONDS, -temple->GetInvestedDiamonds());

        // start exploration
        temple->StartExploring(player->GetFaction(), [this, player, temple]
        {
            mScreen->CenterCameraOverObject(temple);

            ShowDialogExploreTempleOutcome(player, temple);
        });
    });

    TemporaryClosePanels();

    // position dialog
    CenterWidget(mDialogExploreTemple);
}

void GameHUD::HideDialogExploreTemple()
{
    if(nullptr == mDialogExploreTemple)
        return ;

    mScreen->HideScreenOverlay();

    --mVisibleDialogs;

    // delete dialog
    mDialogExploreTemple->DeleteLater();
    mDialogExploreTemple = nullptr;

    ReopenPanels();

    mScreen->SetPause(false);
}

void GameHUD::ShowDialogNewElement(unsigned int type)
{
    if(mDialogNewElement != nullptr)
        return;

    mScreen->ShowScreenOverlay();

    ++mVisibleDialogs;

    mScreen->SetPause(true);

    Game * game = mScreen->GetGame();
    Player * player = game->GetLocalPlayer();

    auto t = static_cast<DialogNewElement::ElemType>(type);
    mDialogNewElement = new DialogNewElement(t, player, game->GetObjectsRegistry());
    mDialogNewElement->SetFocus();

    if(DialogNewElement::ETYPE_STRUCTURES == t)
    {
        // set unit's action to idle while dialog is open
        auto unit = static_cast<Unit *>(player->GetSelectedObject());
        unit->SetActiveAction(GameObjectActionType::IDLE);

        mDialogNewElement->SetFunctionOnBuild([this, unit]
        {
            unit->SetActiveAction(GameObjectActionType::BUILD_STRUCTURE);

            const GameObjectTypeId stype = mDialogNewElement->GetSelectedType();
            unit->SetStructureToBuild(stype);

            HideDialogNewElement();
        });

        mDialogNewElement->SetFunctionOnClose([this, unit]
        {
            unit->SetActiveActionToDefault();
            HideDialogNewElement();
        });
    }
    else
    {
        mDialogNewElement->SetFunctionOnBuild([this, player]
        {
            const GameObjectTypeId type = mDialogNewElement->GetSelectedType();
            mScreen->SetupNewUnit(type, player->GetSelectedObject(), player);

            HideDialogNewElement();
        });

        mDialogNewElement->SetFunctionOnClose([this]
        {
            HideDialogNewElement();
        });
    }

    TemporaryClosePanels();

    // position dialog
    CenterWidget(mDialogNewElement);
}

void GameHUD::HideDialogNewElement()
{
    // no dialog -> nothing to do
    if(nullptr == mDialogNewElement)
        return ;

    mScreen->HideScreenOverlay();

    --mVisibleDialogs;

    ReopenPanels();

    // schedule dialog deletion
    mDialogNewElement->DeleteLater();
    mDialogNewElement = nullptr;

    // un-pause game
    mScreen->SetPause(false);
}

void GameHUD::ShowMissionCountdown(int secs)
{
    const Player * p = mScreen->GetGame()->GetLocalPlayer();
    const PlayerFaction pf = p->GetFaction();
    const auto bases = p->GetStructuresByType(ObjectData::TYPE_BASE);

    // this shouldn't happen
    if(bases.empty())
        return ;

    const Structure * base = bases[0];
    const IsoObject * isoObj = base->GetIsoObject();

    mCountdownLabel = new CountdownLabel(pf, secs, this);

    const int x0 = isoObj->GetX() + (isoObj->GetWidth() - mCountdownLabel->GetWidth()) / 2;
    const int y0 = isoObj->GetY() - mCountdownLabel->GetHeight();

    mCountdownLabel->SetPosition(x0, y0);
}

void GameHUD::HideMissionCountdown()
{
    delete mCountdownLabel;
    mCountdownLabel = nullptr;
}

void GameHUD::ShowGoalCompletedIcon()
{
    using namespace sgl;

    // icon already visible
    if(mGoalCompletedIcon != nullptr)
        return ;

    const Player * p = mScreen->GetGame()->GetLocalPlayer();
    const PlayerFaction pf = p->GetFaction();
    const auto bases = p->GetStructuresByType(ObjectData::TYPE_BASE);

    // this shouldn't happen
    if(bases.empty())
        return ;

    // create icon
    auto tm = graphic::TextureManager::Instance();
    auto tex = tm->GetSprite(SpriteFileGameUI, ID_GAMEUI_GOAL_F1 + pf);

    mGoalCompletedIcon = new sgui::Image(tex, this);
    // set camera to default to follow screen
    mGoalCompletedIcon->SetCamera(graphic::Camera::GetDefaultCamera());

    // position icon over base
    const Structure * base = bases[0];
    const IsoObject * isoObj = base->GetIsoObject();

    const int x = isoObj->GetX() + (isoObj->GetWidth() - mGoalCompletedIcon->GetWidth()) / 2;
    const int y = isoObj->GetY() + (isoObj->GetHeight() - mGoalCompletedIcon->GetHeight()) / 2;

    mGoalCompletedIcon->SetPosition(x, y);

    // play sound
    auto player = media::AudioManager::Instance()->GetPlayer();
    player->PlaySound("UI/goal_completed.ogg");
}

void GameHUD::HideGoalCompletedIcon()
{
    delete mGoalCompletedIcon;
    mGoalCompletedIcon = nullptr;
}

void GameHUD::HidePanelSelectedObject()
{
    mButtonPanelSelObj->SetVisible(false);
    mPanelSelObj->ClearObject();
    mPanelSelObj->SetVisible(false);
}

void GameHUD::ShowPanelSelectedObject(GameObject * obj)
{
    mPanelSelObj->SetObject(obj);

    OpenPanelSelectedObject();
}

void GameHUD::ShowTurnControlPanel()
{
    mPanelTurnCtrl->ShowPanel();
}

void GameHUD::ShowTurnControlText(const char * text)
{
    mPanelTurnCtrl->ShowText(text);
}


void GameHUD::UpdatePanelTurnControl()
{
    auto sm = sgl::utilities::StringManager::Instance();

    // GAME PAUSED
    if(mScreen->mPaused)
    {
        ShowTurnControlText(sm->GetCString("GAME_PAUSED"));
        return ;
    }

    // ENEMY TURN
    if(!mScreen->IsCurrentTurnLocal())
    {
        ShowTurnControlText(sm->GetCString("ENEMY_TURN"));
        return ;
    }

    // LOCAL TURN
    if(TURN_STAGE_MINI_UNITS_MOVE == mScreen->mTurnStage)
        ShowTurnControlText(sm->GetCString("MU_MOVING"));
    else if(TURN_STAGE_MINI_UNITS_ATTACK == mScreen->mTurnStage)
        ShowTurnControlText(sm->GetCString("MU_SHOOTING"));
    else if(TURN_STAGE_TOWERS_ATTACK == mScreen->mTurnStage)
        ShowTurnControlText(sm->GetCString("TOWERS_SHOOTING"));
    else
        ShowTurnControlPanel();
}


void GameHUD::ShowDialogNewMiniUnitsSquad(GameObject * spawner)
{
    if(mDialogNewMiniUnits != nullptr)
        return ;

    mScreen->ShowScreenOverlay();

    ++mVisibleDialogs;

    mScreen->SetPause(true);

    Game * game = mScreen->GetGame();
    mDialogNewMiniUnits = new DialogNewMiniUnitsSquad(spawner, game->GetLocalPlayer(),
                                                      game->GetObjectsRegistry());
    mDialogNewMiniUnits->SetFocus();

    mDialogNewMiniUnits->AddFunctionOnBuild([this, spawner, game]
    {
        mScreen->SetupNewMiniUnits(mDialogNewMiniUnits->GetTypeToBuild(), spawner, nullptr,
                                   game->GetLocalPlayer(), mDialogNewMiniUnits->GetNumSquads(),
                                   mDialogNewMiniUnits->GetNumElements());

        HideDialogNewMiniUnitsSquad();
    });

    mDialogNewMiniUnits->AddFunctionOnClose([this]
    {
        HideDialogNewMiniUnitsSquad();
    });

    TemporaryClosePanels();

    // position dialog
    CenterWidget(mDialogNewMiniUnits);
}

void GameHUD::HideDialogNewMiniUnitsSquad()
{
    --mVisibleDialogs;

    mScreen->HideScreenOverlay();

    ReopenPanels();

    // schedule dialog deletion
    mDialogNewMiniUnits->DeleteLater();
    mDialogNewMiniUnits = nullptr;

    // un-pause game
    mScreen->SetPause(false);
}

void GameHUD::ShowDialogResearch(ResearchCenter * rc)
{
    if(mDialogResearch != nullptr)
        return ;

    mScreen->ShowScreenOverlay();

    ++mVisibleDialogs;

    mScreen->SetPause(true);

    Game * game = mScreen->GetGame();
    mDialogResearch = new DialogResearch(game->GetLocalPlayer(), rc);
    mDialogResearch->SetFocus();

    mDialogResearch->SetFunctionOnClose([this]
    {
        HideDialogResearch();
    });

    TemporaryClosePanels();

    // position dialog
    CenterWidget(mDialogResearch);
}

void GameHUD::HideDialogResearch()
{
    --mVisibleDialogs;

    mScreen->HideScreenOverlay();

    ReopenPanels();

    // schedule dialog deletion
    mDialogResearch->DeleteLater();
    mDialogResearch = nullptr;

    // un-pause game
    mScreen->SetPause(false);
}

void GameHUD::ShowDialogTrading()
{
    if(mDialogTrading != nullptr)
        return ;

    mScreen->ShowScreenOverlay();

    ++mVisibleDialogs;

    mScreen->SetPause(true);

    Game * game = mScreen->GetGame();
    mDialogTrading = new DialogTrading(game, game->GetLocalPlayer());
    mDialogTrading->SetFocus();

    mDialogTrading->SetFunctionOnClose([this]
    {
        HideDialogTrading();
    });

    TemporaryClosePanels();

    // position dialog
    CenterWidget(mDialogTrading);
}

void GameHUD::HideDialogTrading()
{
    --mVisibleDialogs;

    mScreen->HideScreenOverlay();

    ReopenPanels();

    // schedule dialog deletion
    mDialogTrading->DeleteLater();
    mDialogTrading = nullptr;

    // un-pause game
    mScreen->SetPause(false);
}

void GameHUD::ShowDialogUpgrade(GameObject * obj)
{
    if(mDialogUpgrade != nullptr)
        return ;

    mScreen->ShowScreenOverlay();

    ++mVisibleDialogs;

    mScreen->SetPause(true);

    const ObjectsDataRegistry * odr = mScreen->GetGame()->GetObjectsRegistry();
    mDialogUpgrade = new DialogUpgrade(obj, odr);
    mDialogUpgrade->SetFocus();

    mDialogUpgrade->SetFunctionOnClose([this]
    {
        HideDialogUpgrade();
    });

    TemporaryClosePanels();

    // position dialog
    CenterWidget(mDialogUpgrade);
}

void GameHUD::HideDialogUpgrade()
{
    --mVisibleDialogs;

    mScreen->HideScreenOverlay();

    ReopenPanels();

    // schedule dialog deletion
    mDialogUpgrade->DeleteLater();
    mDialogUpgrade = nullptr;

    // un-pause game
    mScreen->SetPause(false);
}

void GameHUD::SetLocalActionsEnabled(bool enabled)
{
    // PANEL OBJECT ACTIONS
    mPanelObjActions->SetActionsEnabled(enabled);

    // PANEL TURN CONTROL
    mPanelTurnCtrl->SetButtonEndTurnEnabled(enabled);

    // QUICK UNIT SELECTION
    const unsigned int numButtons = mScreen->mLocalPlayer->GetNumUnits();

    for(unsigned int i = 0; i < numButtons; ++i)
        mGroupUnitSel->GetButton(i)->SetEnabled(enabled);
}

GameMapProgressBar * GameHUD::CreateProgressBarInCell(const Cell2D & cell, float time, PlayerFaction faction)
{
    GameMapProgressBar * pb = CreateProgressBar(time, faction);

    // set position
    IsoMap * isoMap = mScreen->mIsoMap;
    auto posCell = isoMap->GetCellPosition(cell.row, cell.col);
    const int pbX = posCell.x + (isoMap->GetTileWidth() - pb->GetWidth()) * 0.5f;
    const int pbY = posCell.y + (isoMap->GetTileHeight() * 0.75f - pb->GetHeight());
    pb->SetPosition(pbX, pbY);

    // progress bar visibility depends on local player's visibility map
    pb->SetVisible(mScreen->mGameMap->IsCellVisibleToLocalPlayer(cell.row, cell.col));

    return pb;
}

void GameHUD::HideDialogExploreTempleOutcome()
{
    // no dialog -> nothing to do
    if(nullptr == mDialogExploreTempleOutcome)
        return ;

    mScreen->HideScreenOverlay();

    --mVisibleDialogs;

    ReopenPanels();

    // schedule dialog deletion
    mDialogExploreTempleOutcome->DeleteLater();
    mDialogExploreTempleOutcome = nullptr;

    // un-pause game
    mScreen->SetPause(false);
}

void GameHUD::ShowDialogExploreTempleOutcome(Player * player, Temple * temple)
{
    if(mDialogExploreTempleOutcome != nullptr)
        return ;

    mScreen->ShowScreenOverlay();

    ++mVisibleDialogs;

    mScreen->SetPause(true);

    mDialogExploreTempleOutcome = new DialogExploreTempleOutcome(player, temple);
    mDialogExploreTempleOutcome->SetFocus();

    mDialogExploreTempleOutcome->SetFunctionOnClose([this]
    {
        HideDialogExploreTempleOutcome();
    });

    mDialogExploreTempleOutcome->SetFunctionOnOutcome1([this, player, temple]
    {
        HideDialogExploreTempleOutcome();

        const unsigned int outcome = temple->GetExplorationOutcome1();
        mScreen->mGameMap->HandleTempleExplorationOutcome(outcome, player, temple);
    });

    mDialogExploreTempleOutcome->SetFunctionOnOutcome2([this, player, temple]
    {
        HideDialogExploreTempleOutcome();

        const unsigned int outcome = temple->GetExplorationOutcome2();
        mScreen->mGameMap->HandleTempleExplorationOutcome(outcome, player, temple);
    });

    TemporaryClosePanels();

    // position dialog
    CenterWidget(mDialogExploreTempleOutcome);
}

void GameHUD::HideDialogObject()
{
    if(nullptr == mDialogObj)
        return ;

    mScreen->HideScreenOverlay();

    --mVisibleDialogs;

    // hide dialog
    mDialogObj->DeleteLater();
    mDialogObj = nullptr;

    ReopenPanels();

    sgl::sgui::Stage::Instance()->SetFocus();

    // resume game
    mScreen->SetPause(false);
}

void GameHUD::ShowDialogObject(GameObject * obj)
{
    if(mDialogObj != nullptr)
        return ;

    ++mVisibleDialogs;

    mScreen->ShowScreenOverlay();

    // pause game
    mScreen->SetPause(true);

    // DIALOG OBJECT
    const ObjectsDataRegistry * odr = mScreen->GetGame()->GetObjectsRegistry();
    mDialogObj = new DialogObject(odr);

    mDialogObj->SetFunctionOnClose([this]
        {
            HideDialogObject();
        });

    mDialogObj->SetObject(obj);
    mDialogObj->SetFocus();

    // position dialog
    CenterWidget(mDialogObj);

    TemporaryClosePanels();
}

void GameHUD::ClosePanelMinimap()
{
    mButtonMinimap->SetVisible(true);
    mMiniMap->SetVisible(false);
}

void GameHUD::OpenPanelMinimap()
{
    mButtonMinimap->SetVisible(false);
    mMiniMap->SetVisible(true);
}

void GameHUD::ClosePanelSelectedObject()
{
    mButtonPanelSelObj->SetVisible(true);
    mPanelSelObj->SetVisible(false);
}

void GameHUD::OpenPanelSelectedObject()
{
    mButtonPanelSelObj->SetVisible(false);
    mPanelSelObj->SetVisible(true);
}

void GameHUD::TemporaryClosePanels()
{
    if(mPanelSelObj->IsVisible())
        ClosePanelSelectedObject();

    if(mMiniMap->IsVisible())
        ClosePanelMinimap();

    if(mPanelObjActions->IsVisible())
        mPanelObjActions->SetVisible(false);
}

void GameHUD::ReopenPanels()
{
    if(mButtonPanelSelObj->IsVisible())
        OpenPanelSelectedObject();

    if(mButtonMinimap->IsVisible())
        OpenPanelMinimap();

    if(mPanelObjActions->HasObjectSet())
        mPanelObjActions->SetVisible(true);
}

void GameHUD::PositionOptionsPanelOverObjectActions(sgl::sgui::Widget * panel, unsigned int button)
{
    const int marginB = 15;

    const auto btn = mPanelObjActions->GetButton(static_cast<PanelObjectActions::Button>(button));

    const int panelX = btn->GetScreenX() + (btn->GetWidth() - panel->GetWidth()) / 2;
    const int panelY = btn->GetScreenY() - panel->GetHeight() - marginB;
    panel->SetPosition(panelX, panelY);
}

void GameHUD::ResumeGameFromExit()
{
    if(0 == mVisibleDialogs)
        mScreen->SetPause(false);

    mScreen->GetGame()->GetTutorialManager()->SetTutorialPause(false);
}

GameMapProgressBar * GameHUD::CreateProgressBar(float time, PlayerFaction faction)
{
    GameMapProgressBar * pb = new GameMapProgressBar(faction, time, this);

    // schedule to delete progress bar when done
    pb->AddFunctionOnCompleted([pb]
    {
        pb->DeleteLater();
    });

    return pb;
}

void GameHUD::CenterWidget(sgl::sgui::Widget * w)
{
    auto renderer = sgl::graphic::Renderer::Instance();
    const int posX = (renderer->GetWidth() - w->GetWidth()) / 2;
    const int posY = (renderer->GetHeight() - w->GetHeight()) / 2;
    w->SetPosition(posX, posY);
}

void GameHUD::OnStringsChanged()
{
    UpdatePanelTurnControl();
}

} // namespace game
