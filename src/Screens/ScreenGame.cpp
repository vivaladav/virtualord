#include "Screens/ScreenGame.h"

#include "CameraMapController.h"
#include "Game.h"
#include "GameConstants.h"
#include "GameData.h"
#include "GameMap.h"
#include "IsoLayer.h"
#include "IsoMap.h"
#include "MapIO.h"
#include "MapsRegistry.h"
#include "Player.h"
#include "AI/ConquerPath.h"
#include "AI/ObjectPath.h"
#include "AI/PlayerAI.h"
#include "AI/WallBuildPath.h"
#include "GameObjects/Base.h"
#include "GameObjects/DefensiveTower.h"
#include "GameObjects/Hospital.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "GameObjects/Temple.h"
#include "GameObjects/Unit.h"
#include "GameObjects/WallGate.h"
#include "Indicators/AttackRangeIndicator.h"
#include "Indicators/ConquestIndicator.h"
#include "Indicators/HealingRangeIndicator.h"
#include "Indicators/MoveIndicator.h"
#include "Indicators/StructureIndicator.h"
#include "Indicators/WallIndicator.h"
#include "Particles/UpdaterDamage.h"
#include "Particles/UpdaterHealing.h"
#include "Particles/UpdaterLootboxPrize.h"
#include "Particles/UpdaterSingleLaser.h"
#include "States/StatesIds.h"
#include "Tutorial/StepDelay.h"
#include "Tutorial/StepGameBase.h"
#include "Tutorial/StepGameBaseBuildUnit.h"
#include "Tutorial/StepGameBaseBuildUnitIcon.h"
#include "Tutorial/StepGameBaseFeatures.h"
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
#include "Tutorial/StepGameStructConnected.h"
#include "Tutorial/StepGameStructDisconnected.h"
#include "Tutorial/StepGameTurnEnergy.h"
#include "Tutorial/StepGameUnit.h"
#include "Tutorial/StepGameUnitConquerCellsIcon.h"
#include "Tutorial/StepGameUnitSelect.h"
#include "Tutorial/StepGameWaitTurn.h"
#include "Tutorial/TutorialManager.h"
#include "Widgets/ButtonQuickUnitSelection.h"
#include "Widgets/DialogNewElement.h"
#include "Widgets/GameHUD.h"
#include "Widgets/GameMapProgressBar.h"
#include "Widgets/MiniMap.h"
#include "Widgets/PanelObjectActions.h"

#include <sgl/ai/Pathfinder.h>
#include <sgl/core/event/ApplicationEvent.h>
#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/core/event/MouseButtonEvent.h>
#include <sgl/core/event/MouseMotionEvent.h>
#include <sgl/graphic/Camera.h>
#include <sgl/graphic/ParticlesManager.h>
#include <sgl/graphic/Renderer.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/Stage.h>

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace game
{

// NOTE these will be replaced by dynamic values soon
constexpr float TIME_NEW_UNIT = 2.f;
constexpr float TIME_CONQ_RES_GEN = 2.f;
constexpr float TIME_UPG_UNIT = 5.f;

constexpr float TIME_ENERGY_USE = 8.f;
constexpr float TIME_AUTO_END_TURN = 2.f;

ScreenGame::ScreenGame(Game * game)
    : Screen(game)
    , mPartMan(new sgl::graphic::ParticlesManager)
    , mPathfinder(new sgl::ai::Pathfinder)
    , mCurrCell(-1, -1)
    , mTimerAutoEndTurn(TIME_AUTO_END_TURN)
    , mLocalPlayer(game->GetLocalPlayer())
{
    game->SetClearColor(0x1A, 0x1A, 0x1A, 0xFF);

    const int rendW = sgl::graphic::Renderer::Instance()->GetWidth();
    const int rendH = sgl::graphic::Renderer::Instance()->GetHeight();

    auto cam = sgl::graphic::Camera::GetDefaultCamera();
    cam->SetSize(rendW, rendH);

    mCamController = new CameraMapController(cam, game);
    mCamController->SetDraggingSpeed(game->GetMapDraggingSpeed());
    mCamController->SetScrollingSpeed(game->GetMapScrollingSpeed());

    mIdOnSettingsChanged = game->AddOnSettingsChangedFunction([this, game]
    {
        mCamController->SetDraggingSpeed(game->GetMapDraggingSpeed());
        mCamController->SetScrollingSpeed(game->GetMapScrollingSpeed());
    });

    InitParticlesSystem();

    // -- ISOMETRIC MAP --
    CreateIsoMap();
    CreateLayers();

    // create game map
    mGameMap = new GameMap(GetGame(), this, mIsoMap);

    LoadMapFile();

    TrackResourcesForGoals();

    // center map on screen
    const int mapH = mIsoMap->GetHeight();

    mIsoMap->SetOrigin(rendW * 0.5, (rendH - mapH) * 0.5);
    mIsoMap->SetVisibleArea(cam->GetX(), cam->GetY(), cam->GetWidth(), cam->GetHeight());

    cam->SetFunctionOnMove([this]
    {
        const sgl::graphic::Camera * cam = mCamController->GetCamera();
        const int camW = cam->GetWidth();
        const int camH = cam->GetHeight();
        const int camX0 = cam->GetX();
        const int camY0 = cam->GetY();
        const int camX1 = camX0 + camW;
        const int camY1 = camY0 + camH;

        // update map
        mIsoMap->SetVisibleArea(camX0, camY0, camW, camH);

        // update MiniMap
        MiniMap * mm = mHUD->GetMinimap();
        mm->SetCameraCells(mIsoMap->CellFromWorldPoint(camX0, camY0),
                           mIsoMap->CellFromWorldPoint(camX1, camY0),
                           mIsoMap->CellFromWorldPoint(camX0, camY1),
                           mIsoMap->CellFromWorldPoint(camX1, camY1));
    });

    // set reduced map area to cam controller so camera will stop closer to inside cells
    const sgl::core::Pointd2D isoMapO = mIsoMap->GetOrigin();
    const int isoMapHalfW = mIsoMap->GetWidth() / 2;
    const int isoMapHalfH = mIsoMap->GetHeight() / 2;
    const int marginCameraMult = 2;
    const int tileW = mIsoMap->GetTileWidth();
    const int tileH = mIsoMap->GetTileHeight();
    const int marginCameraX = marginCameraMult * tileW;
    const int marginCameraY = marginCameraMult * tileH;

    const sgl::core::Pointd2D pT(isoMapO.x, isoMapO.y + marginCameraY);
    const sgl::core::Pointd2D pR(pT.x + isoMapHalfW - marginCameraX, pT.y + isoMapHalfH);
    const sgl::core::Pointd2D pB(pT.x, pT.y + mIsoMap->GetHeight() - marginCameraY);
    const sgl::core::Pointd2D pL(pT.x - isoMapHalfW + marginCameraX, pT.y + isoMapHalfH);
    mCamController->SetMapArea(pT, pR, pB, pL);

    // init pathfinder
    mPathfinder->SetMap(mGameMap, mGameMap->GetNumRows(), mGameMap->GetNumCols());

    // -- PLAYERS --
    for(int i = 0; i < GetGame()->GetNumPlayers(); ++i)
    {
        Player * p = game->GetPlayerByIndex(i);

        p->ResetTurnEnergy();

        if(p->IsAI())
        {
            p->GetAI()->SetGameMap(mGameMap);
            mAiPlayers.push_back(p);
        }
    }

    // UI
    CreateUI();

    // set initial camera position
    CenterCameraOverPlayerBase();

    // apply initial visibility to the game map
    mGameMap->InitVisibility(mLocalPlayer);

    InitMusic();

    // TUTORIAL
    if(game->IsTutorialEnabled() && game->GetTutorialState(TUTORIAL_MISSION_INTRO) == TS_TODO)
        CreateTutorial();
}

ScreenGame::~ScreenGame()
{
    ClearResourcesTracking();

    // clear Players
    Game * game = GetGame();

    for(int i = 0; i < game->GetNumPlayers(); ++i)
    {
        Player * p = game->GetPlayerByIndex(i);
        p->ClearMissionObjects();
        p->ClearSelectedObject();
    }

    game->RemoveOnSettingsChangedFunction(mIdOnSettingsChanged);

    delete mPathfinder;
    delete mPartMan;

    for(auto ind : mAttIndicators)
        delete ind;

    for(auto ind : mHealIndicators)
        delete ind;

    for(auto ind : mConquestIndicators)
        delete ind;

    for(auto ind : mWallIndicators)
        delete ind;

    for(auto it : mStructIndicators)
        delete it.second;

    delete mTempStructIndicator;

    delete mIsoMap;
    delete mGameMap;

    delete mCamController;

    auto stage = sgl::sgui::Stage::Instance();
    stage->ClearWidgets();
    // make sure to reset stage visibility in case it was off before exit
    stage->SetVisible(true);
}

unsigned int ScreenGame::GetPlayTimeInSec() const
{
    return std::roundf(mTimePlayed);
}

void ScreenGame::Update(float delta)
{
    Game * game = GetGame();

    // do nothing when paused
    if(mPaused)
    {
        // only continue the tutorial if paused
        if(mTutMan != nullptr)
            UpdateTutorial(delta);

        return ;
    }

    // keep track of time played (while not paused)
    mTimePlayed += delta;

    // -- CAMERA --
    mCamController->Update(delta);

    // -- PARTICLES --
    mPartMan->Update(delta);

    // -- AUTO END TURN --
    const float minEn = 1.f;

    if(IsCurrentTurnLocal() && game->IsAutoEndTurnEnabled() &&
       mLocalPlayer->GetTurnEnergy() < minEn)
    {
        mTimerAutoEndTurn -= delta;

        if(mTimerAutoEndTurn <= 0.f)
        {
            EndTurn();

            mTimerAutoEndTurn = TIME_AUTO_END_TURN;
        }
    }

    // -- GAME MAP AND OBJECTS --
    // merge object actions to do with object actions list
    mObjActions.insert(mObjActions.end(), mObjActionsToDo.begin(), mObjActionsToDo.end());
    mObjActionsToDo.clear();

    mGameMap->Update(delta);

    // -- AI --
    if(!mAiPlayers.empty())
         UpdateAI(delta);

    // TUTORIAL
    if(mTutMan != nullptr)
        UpdateTutorial(delta);

    // check game end
    UpdateGameEnd();
}

void ScreenGame::Render()
{
    mIsoMap->Render();

    mPartMan->Render();
}

void ScreenGame::ClearObjectAction(GameObject * obj)
{
    auto it = mObjActions.begin();

    // search selected object in active actions
    while(it != mObjActions.end())
    {
        if(it->obj == obj)
        {
            if(it->progressBar != nullptr)
                it->progressBar->DeleteLater();

            mObjActions.erase(it);

            return ;
        }

        ++it;
    }
}

sgl::graphic::ParticlesUpdater * ScreenGame::GetParticleUpdater(ParticlesUpdaterId updaterId)
{
    return mPartMan->GetUpdater(updaterId);
}

void ScreenGame::ClearSelection(Player * player)
{
    GameObject * selObj = player->GetSelectedObject();

    // no selection -> nothing to do
    if(nullptr == selObj)
        return ;

    mHUD->ClearQuickUnitButtonChecked();
    mHUD->HidePanelObjectActions();
    mHUD->HidePanelSelectedObject();
    mHUD->HideDialogNewElement();

    player->ClearSelectedObject();

    ClearCellOverlays();
}

void ScreenGame::SelectObject(GameObject * obj, Player * player)
{
    player->SetSelectedObject(obj);

    // update quick selection buttons when selected unit
    if(obj->GetObjectCategory() == GameObject::CAT_UNIT)
    {
        mHUD->SetQuickUnitButtonChecked(obj);

        // show current indicator
        ShowActiveIndicators(static_cast<Unit *>(obj), mCurrCell);
    }
    // not a unit
    else
    {
        mHUD->ClearQuickUnitButtonChecked();

        // show attack range overlay for towers
        if(GameObject::TYPE_DEFENSIVE_TOWER == obj->GetObjectType())
        {
            auto tower = static_cast<DefensiveTower *>(obj);
            const int range = tower->GetAttackRange();
            ShowAttackIndicators(tower, range);
        }
    }

    mHUD->ShowPanelObjectActions(obj);
    mHUD->ShowPanelSelectedObject(obj);

    sgl::sgui::Stage::Instance()->SetFocus();
}

void ScreenGame::CenterCameraOverCell(int row, int col)
{
    const sgl::core::Pointd2D pos = mIsoMap->GetCellPosition(row, col);
    const int cX = pos.x + mIsoMap->GetTileWidth() * 0.5f;
    const int cY = pos.y + mIsoMap->GetTileHeight() * 0.5f;

    mCamController->CenterCameraToPoint(cX, cY);
}

void ScreenGame::CenterCameraOverObject(GameObject * obj)
{
    const GameMapCell * cell = obj->GetCell();
    CenterCameraOverCell(cell->row, cell->col);

    // update current cell like if mouse was moved
    UpdateCurrentCell();
}

MiniMap * ScreenGame::GetMiniMap() const
{
    if(mHUD)
        return mHUD->GetMinimap();
    else
        return nullptr;
}

void ScreenGame::SetMiniMapEnabled(bool val)
{
    mHUD->SetMiniMapEnabled(val);
}

void ScreenGame::SetPause(bool paused)
{
    mPaused = paused;

    mHUD->SetEnabled(!paused);

    // handle turn control panel text
    if(paused)
        mHUD->ShowTurnControlTextGamePaused();
    else
    {
        if(IsCurrentTurnLocal())
            mHUD->ShowTurnControlPanel();
        else
            mHUD->ShowTurnControlTextEnemyTurn();
    }
}

void ScreenGame::CollectMissionGoalReward(unsigned int index)
{
    if(index >= mMissionGoals.size())
        return ;

    MissionGoal & g = mMissionGoals[index];

    if(g.IsRewardCollected())
        return ;

    const Player::Stat resourceIds[NUM_MISSION_REWARDS] =
    {
        Player::BLOBS,
        Player::DIAMONDS,
        Player::ENERGY,
        Player::MATERIAL,
        Player::MONEY
    };

    const MissionReward rewardIds[NUM_MISSION_REWARDS] =
    {
        MR_BLOBS,
        MR_DIAMONDS,
        MR_ENERGY,
        MR_MATERIAL,
        MR_MONEY,
    };

    for(unsigned int i = 0; i < NUM_MISSION_REWARDS; ++i)
    {
        const int reward = g.GetRewardByType(rewardIds[i]);

        if(reward > 0)
            mLocalPlayer->SumResource(resourceIds[i], reward);
    }

    g.SetRewardCollected();

    UpdateGoalCompletedIcon();
}

void ScreenGame::OnApplicationQuit(sgl::core::ApplicationEvent & event)
{
    mHUD->ShowDialogExit();

    event.SetConsumed();
}

void ScreenGame::InitMusic()
{
    auto am = sgl::media::AudioManager::Instance();
    auto ap = am->GetPlayer();

    ap->AddMusicToQueue("mission/music_01.ogg");
    ap->AddMusicToQueue("mission/music_02.ogg");
    ap->PlayMusicQueue();
}

void ScreenGame::InitParticlesSystem()
{
    sgl::graphic::ParticlesUpdater * updater;

    // DAMAGE
    updater = new UpdaterDamage;
    mPartMan->RegisterUpdater(PU_DAMAGE, updater);

    // HEALING
    updater = new UpdaterHealing;
    mPartMan->RegisterUpdater(PU_HEALING, updater);

    // LOOTBOX PRIZE
    updater = new UpdaterLootboxPrize;
    mPartMan->RegisterUpdater(PU_LOOTBOX_PRIZE, updater);

    // SINGLE LASER
    updater = new UpdaterSingleLaser;
    mPartMan->RegisterUpdater(PU_SINGLE_LASER, updater);
}

void ScreenGame::CreateIsoMap()
{
    const int TILE_W = 96;

    // iso map
    mIsoMap = new IsoMap(TILE_W);
    mIsoMap->SetTiles(SpriteFileCells, NUM_IND_CELLS);
}

void ScreenGame::CreateLayers()
{
    mIsoMap->CreateLayer(MapLayers::CELL_OVERLAYS1);
    mIsoMap->CreateLayer(MapLayers::CELL_OVERLAYS2);
    mIsoMap->CreateLayer(MapLayers::CELL_OVERLAYS3);
    mIsoMap->CreateLayer(MapLayers::FACTION_INFLUENCE);
    mIsoMap->CreateLayer(MapLayers::OBJECTS1);
    mIsoMap->CreateLayer(MapLayers::OBJECTS2);
    mIsoMap->CreateLayer(MapLayers::CELL_OVERLAYS4);
}

void ScreenGame::CreateUI()
{
    using namespace sgl;

    const int rendW = graphic::Renderer::Instance()->GetWidth();
    const int rendH = graphic::Renderer::Instance()->GetHeight();

    // init HUD layer
    mHUD = new GameHUD(this);

    mHUD->SetMiniMapEnabled(false);

    PanelObjectActions * panelObjActions = mHUD->GetPanelObjectActions();

    // create new unit
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_BUILD_UNIT_BARRACKS,
        [this, panelObjActions]
    {
        mHUD->ShowDialogNewElement(DialogNewElement::ETYPE_UNITS_BARRACKS);
    });

    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_BUILD_UNIT_BASE,
        [this, panelObjActions]
    {
        mHUD->ShowDialogNewElement(DialogNewElement::ETYPE_UNITS_BASE);
    });

    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_BUILD_UNIT_HOSPITAL,
        [this, panelObjActions]
    {
        mHUD->ShowDialogNewElement(DialogNewElement::ETYPE_UNITS_HOSPITAL);
    });

    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_MISSION_GOALS,
    [this, panelObjActions]
    {
        mHUD->ShowDialogMissionGoals();
    });

    // UNIT ACTIONS
    // build structure
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_BUILD_STRUCT,
        [this, panelObjActions]
    {
        mHUD->ShowDialogNewElement(DialogNewElement::ETYPE_STRUCTURES);

        ClearCellOverlays();
    });

    // build wall
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_BUILD_WALL, [this]
    {
        auto unit = static_cast<Unit *>(mLocalPlayer->GetSelectedObject());
        unit->SetActiveAction(GameObjectActionType::BUILD_WALL);

        ClearCellOverlays();

        mWallPath.clear();

        ShowBuildWallIndicator(unit, mCurrCell);
    });

    // attack
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_ATTACK, [this]
    {
        auto unit = static_cast<Unit *>(mLocalPlayer->GetSelectedObject());
        unit->SetActiveAction(GameObjectActionType::ATTACK);

        ClearCellOverlays();

        // show attack range overlay
        const int range = unit->GetRangeAttack();
        ShowAttackIndicators(unit, range);
    });

    // heal
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_HEAL_HOSPITAL, [this]
    {
        auto hospital = static_cast<Hospital *>(mLocalPlayer->GetSelectedObject());
        hospital->SetActiveAction(GameObjectActionType::HEAL);

        ClearCellOverlays();

        // show healing range overlay
        const int range = hospital->GetRangeHealing();
        ShowHealingIndicators(hospital, range);
    });

    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_HEAL_UNIT, [this]
    {
        auto unit = static_cast<Unit *>(mLocalPlayer->GetSelectedObject());
        unit->SetActiveAction(GameObjectActionType::HEAL);

        ClearCellOverlays();

        // show healing range overlay
        const int range = unit->GetRangeHealing();
        ShowHealingIndicators(unit, range);
    });

    // conquer
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_CONQUER_CELL, [this]
    {
        auto unit = static_cast<Unit *>(mLocalPlayer->GetSelectedObject());
        unit->SetActiveAction(GameObjectActionType::CONQUER_CELL);

        ClearCellOverlays();

        mConquestPath.clear();

        ShowConquestIndicator(unit, mCurrCell);
    });

    // move
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_MOVE, [this]
    {
        auto unit = static_cast<Unit *>(mLocalPlayer->GetSelectedObject());
        unit->SetActiveAction(GameObjectActionType::MOVE);

        ClearCellOverlays();

        ShowMoveIndicator(unit, mCurrCell);
    });

    // WALL GATE
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_OPEN_GATE,
                                       [this, panelObjActions]
    {
        // open gate
        auto gate = static_cast<WallGate *>(mLocalPlayer->GetSelectedObject());
        const bool res = gate->Toggle();

        if(!res)
            return ;

        // move object down in game map
        mGameMap->MoveObjectDown(gate);

        // move to iso layer 1
        mIsoMap->ChangeObjectLayer(gate->GetIsoObject(), MapLayers::OBJECTS2, MapLayers::OBJECTS1);

        // update panel actions
        panelObjActions->SetObject(gate);

        // reset focus as buttons will change
        sgl::sgui::Stage::Instance()->SetFocus();
    });

    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_CLOSE_GATE,
                                       [this, panelObjActions]
    {
        // close gate
        auto gate = static_cast<WallGate *>(mLocalPlayer->GetSelectedObject());
        const bool res = gate->Toggle();

        if(!res)
            return ;

        // move object up in game map
        mGameMap->MoveObjectUp(gate);

        // move to iso layer 2
        mIsoMap->ChangeObjectLayer(gate->GetIsoObject(), MapLayers::OBJECTS1, MapLayers::OBJECTS2);

        // update panel actions
        panelObjActions->SetObject(gate);

        // reset focus as buttons will change
        sgl::sgui::Stage::Instance()->SetFocus();
    });

    // TRADING POST
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_TRADE, [this]
    {
        mHUD->ShowDialogTrading();
    });

    // GENERIC ACTIONS
    // upgrade
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_UPGRADE, [this]
    {
        // TODO

        ClearCellOverlays();
    });

    // cancel
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_CANCEL,
                                       [this, panelObjActions]
    {
        GameObject * selObj = mLocalPlayer->GetSelectedObject();

        if(nullptr == selObj)
            return ;

        const GameObjectActionType action = selObj->GetActiveAction();

        if(action == CONQUER_CELL || action == BUILD_WALL)
        {
            // clear overlays
            ClearCellOverlays();
            mConquestPath.clear();
            mWallPath.clear();

            // reset object action
            selObj->SetCurrentAction(GameObjectActionType::IDLE);
            selObj->SetActiveActionToDefault();

            // show current indicator
            ShowActiveIndicators(static_cast<Unit *>(selObj), mCurrCell);

            return ;
        }

        CancelObjectAction(selObj);
    });

    // MISSION COUNTDOWN if needed by primary goal
    for(const MissionGoal & g : mMissionGoals)
    {
        if(g.IsPrimary() && MG_RESIST_TIME == g.GetType())
        {
            mHUD->ShowMissionCountdown(g.GetQuantity());
            break;
        }
    }

    // set initial focus to Stage
    sgl::sgui::Stage::Instance()->SetFocus();
}

void ScreenGame::CreateTutorial()
{
    Game * game = GetGame();
    Player * local = game->GetLocalPlayer();

    auto panelActions = mHUD->GetPanelObjectActions();
    auto panelObj = mHUD->GetPanelSelectedObject();
    auto panelTurn = mHUD->GetPanelTurnControl();

    mTutMan = new TutorialManager;
    mTutMan->AddStep(new StepGameDisableCamera(mCamController));

    mTutMan->AddStep(new StepDelay(1.f));
    mTutMan->AddStep(new StepGameIntro);
    mTutMan->AddStep(new StepDelay(0.3f));
    mTutMan->AddStep(new StepGameBase(local->GetBase()));
    mTutMan->AddStep(new StepDelay(0.5f));
    mTutMan->AddStep(new StepGameBaseFeatures(panelObj, panelActions));
    mTutMan->AddStep(new StepGameMissionGoalsIcon(panelActions));
    mTutMan->AddStep(new StepDelay(0.5f));
    mTutMan->AddStep(new StepGameMissionGoalsDialog(mHUD));
    mTutMan->AddStep(new StepDelay(0.5f));
    mTutMan->AddStep(new StepGameBaseBuildUnitIcon(panelActions));
    mTutMan->AddStep(new StepDelay(0.5f));
    mTutMan->AddStep(new StepGameBaseBuildUnit(mHUD));
    // TODO replace constant with time from Base when implemented
    mTutMan->AddStep(new StepDelay(TIME_NEW_UNIT));
    mTutMan->AddStep(new StepGameUnit(local));
    mTutMan->AddStep(new StepDelay(0.5f));
    mTutMan->AddStep(new StepGameMoveUnit(local, mIsoMap));
    mTutMan->AddStep(new StepDelay(3.f));
    const int genR = 56;
    const int genC = 13;
    const GameMapCell gmc = mGameMap->GetCell(genR, genC);
    mTutMan->AddStep(new StepGameMoveCamera(200, -100));
    mTutMan->AddStep(new StepGameConquerStruct(gmc.objTop, mIsoMap));
    mTutMan->AddStep(new StepDelay(0.5f));
    mTutMan->AddStep(new StepGameTurnEnergy(mHUD));
    mTutMan->AddStep(new StepDelay(0.5f));
    mTutMan->AddStep(new StepGameEndTurn(panelTurn));
    mTutMan->AddStep(new StepGameWaitTurn(this));
    mTutMan->AddStep(new StepDelay(0.5f));
    mTutMan->AddStep(new StepGameEnergyRegeneration);
    mTutMan->AddStep(new StepGameStructDisconnected);
    mTutMan->AddStep(new StepGameUnitSelect(local));
    mTutMan->AddStep(new StepGameUnitConquerCellsIcon(panelActions));
    mTutMan->AddStep(new StepGameConquerCells(local, mIsoMap));
    mTutMan->AddStep(new StepDelay(0.5f));
    mTutMan->AddStep(new StepGameStructConnected);

    mTutMan->AddStep(new StepGameEnableCamera(mCamController));

    mTutMan->AddStep(new StepGameMapNavigation);

    game->SetTutorialState(TUTORIAL_MISSION_INTRO, TS_IN_PROGRESS);

    mTutMan->Start();
}

void ScreenGame::UpdateTutorial(float delta)
{
    mTutMan->Update(delta);

    if(mTutMan->AreAllStepsDone())
    {
        GetGame()->SetTutorialState(TUTORIAL_MISSION_INTRO, TS_DONE);

        delete mTutMan;
        mTutMan = nullptr;
    }
}

void ScreenGame::LoadMapFile()
{
    const std::string & mapFile = GetGame()->GetCurrentMapFile();

    MapIO mio;
    mio.Load(mapFile);

    const unsigned int rows = mio.GetMapRows();
    const unsigned int cols = mio.GetMapCols();

    // update iso map
    mIsoMap->SetSize(rows, cols, true);

    // update game map
    mGameMap->SetSize(rows, cols);

    const std::vector<unsigned int> & cells = mio.GetCellTypes();

    for(unsigned int r = 0; r < rows; ++r)
    {
        const unsigned int ind0 = r * cols;

        for(unsigned int c = 0; c < cols; ++c)
        {
            const unsigned int ind = ind0 + c;
            const auto t = static_cast<CellTypes>(cells[ind]);
            mGameMap->SetCellType(ind, t);

            // create collectable generators
            if(t == BLOBS_SOURCE)
                mGameMap->CreateCollectableGenerator(r, c, RES_BLOBS);
            else if(t == DIAMONDS_SOURCE)
                mGameMap->CreateCollectableGenerator(r, c, RES_DIAMONDS);
        }
    }

    // create objects
    const std::vector<MapObjectEntry> & objEntries = mio.GetObjectEntries();
    const unsigned int numEntries = objEntries.size();

    for(unsigned int i = 0; i < numEntries; ++i)
    {
        const auto & e = objEntries[i];

        mGameMap->CreateObjectFromFile(e.layerId, e.typeId, e.variantId, e.faction, e.r0, e.c0);
    }

    // get mission data
    mMissionGoals = mio.GetMissionGoals();
    SetMissionRewards();


}

void ScreenGame::OnKeyDown(sgl::core::KeyboardEvent & event)
{
    mCamController->HandleKeyDown(event);
}

void ScreenGame::OnKeyUp(sgl::core::KeyboardEvent & event)
{
    using namespace sgl::core;

    // CAMERA
    mCamController->HandleKeyUp(event);

    UpdateCurrentCell();

    // GAME
    const int key = event.GetKey();

    // P -> PAUSE
    if(key == KeyboardEvent::KEY_P)
        SetPause(!mPaused);
    else if(key == KeyboardEvent::KEY_ESCAPE)
        mHUD->ShowDialogExit();
    // SHIFT + B -> center camera on own base
    else if(key == KeyboardEvent::KEY_B)
    {
        if(event.IsModShiftDown())
            CenterCameraOverPlayerBase();
    }
#ifdef DEV_MODE
    // DEBUG: ALT + U -> toggle UI
    else if(event.IsModAltDown() && key == KeyboardEvent::KEY_U)
        mHUD->SetVisible(!mHUD->IsVisible());
    // DEBUG: SHIFT/CTRL + V -> add/remove visibility to whole map
    else if(key == KeyboardEvent::KEY_V)
    {
        if(event.IsModShiftDown())
        {
            mLocalPlayer->AddVisibilityToAll();
            mGameMap->ApplyVisibility(mLocalPlayer);
        }
        else if(event.IsModCtrlDown())
        {
            mLocalPlayer->RemVisibilityToAll();
            mGameMap->ApplyVisibility(mLocalPlayer);
        }
    }
    // DEBUG: end mission dialog win/lose
    else if(event.IsModCtrlDown() && key == KeyboardEvent::KEY_W)
        mHUD->ShowDialogEndMission(true);
    else if(event.IsModCtrlDown() && key == KeyboardEvent::KEY_L)
        mHUD->ShowDialogEndMission(false);
    // DEBUG: explore temple dialog
    else if(event.IsModCtrlDown() && key == KeyboardEvent::KEY_E)
    {
        auto objs = mGameMap->GetObjects();

        for(GameObject * o : objs)
        {
            // assign first Temple found
            if(o->GetObjectType() == GameObject::TYPE_TEMPLE)
            {
                mHUD->ShowDialogExploreTemple(mLocalPlayer, static_cast<Temple *>(o));

                break;
            }
        }
    }
    // DEBUG: show dialog trading
    else if(event.IsModShiftDown() && key == KeyboardEvent::KEY_T)
        mHUD->ShowDialogTrading();
#endif
}

void ScreenGame::OnMouseButtonUp(sgl::core::MouseButtonEvent & event)
{
    // CAMERA
    const bool wasDragging = mCamController->IsDragging();

    mCamController->HandleMouseButtonUp(event);

    // do nothing if dragging
    if(wasDragging)
        return;

    // no interaction while game is paused
    if(mPaused)
        return ;

    // no interaction during enemy turn
    if(!IsCurrentTurnLocal())
        return ;

    if(event.GetButton() == sgl::core::MouseEvent::BUTTON_LEFT)
        HandleSelectionClick(event);
    else if(event.GetButton() == sgl::core::MouseEvent::BUTTON_RIGHT)
        HandleActionClick(event);
}

void ScreenGame::OnMouseMotion(sgl::core::MouseMotionEvent & event)
{
    mMousePos.x = event.GetX();
    mMousePos.y = event.GetY();

    // CAMERA
    mCamController->HandleMouseMotion(event);

    // no interaction during enemy turn
    if(!IsCurrentTurnLocal())
        return ;

    UpdateCurrentCell();
}

void ScreenGame::OnWindowExposed(sgl::graphic::WindowEvent &)
{
    SetPause(false);
}

void ScreenGame::OnWindowHidden(sgl::graphic::WindowEvent &)
{
    SetPause(true);
}

void ScreenGame::OnWindowMouseEntered(sgl::graphic::WindowEvent & event)
{
}

void ScreenGame::OnWindowMouseLeft(sgl::graphic::WindowEvent & event)
{
    mCamController->HandleMouseLeftWindow();
}

void ScreenGame::UpdateAI(float delta)
{
    // nothing to do during local player turn
    if(IsCurrentTurnLocal())
        return ;

    // convert player playing turn to AI index
    const int turnAI = mActivePlayerIdx - 1;

    PlayerAI * ai = mAiPlayers[turnAI]->GetAI();
    Player * player = ai->GetPlayer();

    // already doing action -> wait
    if(ai->IsDoingSomething())
        return ;

    std::cout << "\n----- ScreenGame::UpdateAI " << turnAI << " -----\n"
              << "MONEY: " << player->GetStat(Player::MONEY).GetValue()
              << " - ENERGY: " << player->GetStat(Player::ENERGY).GetValue() << " / "
              << player->GetStat(Player::ENERGY).GetMax()
              << " - MATERIAL: " << player->GetStat(Player::MATERIAL).GetValue() << "/"
              << player->GetStat(Player::MATERIAL).GetMax()
              << " - BLOBS: " << player->GetStat(Player::BLOBS).GetValue() << "/"
              << player->GetStat(Player::BLOBS).GetMax()
              << " - DIAMONDS: " << player->GetStat(Player::DIAMONDS).GetValue() << "/"
              << player->GetStat(Player::DIAMONDS).GetMax()
              << std::endl;

    // no more turn energy -> end turn
    const float minEnergy = 1.f;

    if(player->GetTurnEnergy() < minEnergy)
    {
        std::cout << "ScreenGame::UpdateAI - AI " << turnAI
                  << " ==================== END TURN ====================" << std::endl;

        EndTurn();
        return ;
    }

    // make AI decide what to do
    ai->DecideNextAction();
    std::cout << std::endl;
    ExecuteAIAction(ai);

    std::cout << "----------------------------------------\n" << std::endl;
}

void ScreenGame::ExecuteAIAction(PlayerAI * ai)
{
    // convert player playing turn to AI index
    const int turnAI = mActivePlayerIdx - 1;

    bool done = false;

    Player * player = ai->GetPlayer();

    auto PrintAction = [](int turnAI, const ActionAI * action, bool done, Player * player)
    {
        std::cout << "ScreenGame::ExecuteAIAction - AI " << turnAI << " - "
                  << action->GetTypeStr()
                  << (done ? " DOING" : " FAILED")
                  << " | ACT ID: " << action->actId
                  << " - PRIORITY: " << action->priority
                  << " | OBJ ID: " << action->ObjSrc->GetObjectId()
                  << " - OBJ ENERGY: " << action->ObjSrc->GetEnergy()
                  << " - TURN ENERGY: " << player->GetTurnEnergy() << std::endl;
    };

    // execute planned action until one is successful or there's no more actions to do (NOP)
    while(!done)
    {
        const ActionAI * action = ai->GetNextActionTodo();

        if(nullptr == action || AIA_END_TURN == action->type)
        {
            std::cout << "ScreenGame::ExecuteAIAction - AI " << turnAI
                      << " ==================== END TURN ====================" << std::endl;

            EndTurn();

            return ;
        }

        auto basicOnDone = [action, ai](bool)
        {
            ai->SetActionDone(action);
        };

        // new higher action for busy object
        if(action->ObjSrc->IsBusy() && ai->IsActionHighestPriorityForObject(action))
        {
            std::cout << "ScreenGame::ExecuteAIAction - AI " << turnAI
                      << " - higher priority action for object" << std::endl;

            // can/cel current action
            ai->CancelObjectAction(action->ObjSrc);
            CancelObjectAction(action->ObjSrc);
        }

        switch(action->type)
        {
            case AIA_UNIT_ATTACK_ENEMY_UNIT:
            {
                auto unit = static_cast<Unit *>(action->ObjSrc);
                done = SetupUnitAttack(unit, action->ObjDst, player, basicOnDone);

                PrintAction(turnAI, action, done, player);
            }
            break;

            case AIA_UNIT_CONQUER_GEN:
            {
                auto unit = static_cast<Unit *>(action->ObjSrc);
                const Cell2D start(unit->GetRow0(), unit->GetCol0());
                const Cell2D end(action->ObjDst->GetRow0(), action->ObjDst->GetCol0());

                // NOTE probably need to change actions to not take into consideration cells,
                // but only objects

                // unit and generator are next to each other
                if(mGameMap->AreObjectsOrthoAdjacent(action->ObjSrc, action->ObjDst))
                {
                    std::cout << "ScreenGame::ExecuteAIAction - AI " << turnAI
                              << " - CONQUER GENERATOR ADJACENT" << std::endl;

                    done = SetupStructureConquest(unit, start, end, player, basicOnDone);
                }
                // unit needs to move to the generator
                else
                {
                    Cell2D target = mGameMap->GetOrthoAdjacentMoveTarget(start, action->ObjDst);

                    // failed to find a suitable target
                    if(-1 == target.row || -1 == target.col)
                    {
                        std::cout << "ScreenGame::ExecuteAIAction - CONQUER GENERATOR -"
                                     "GetOrthoAdjacentMoveTarget FAILED" << std::endl;

                        done = false;
                    }
                    else
                    {
                        done = SetupUnitMove(unit, start, target,
                            [this, unit, end, player, basicOnDone](bool successful)
                            {
                                if(successful)
                                {
                                    std::cout << "ScreenGame::ExecuteAIAction - CONQUER GENERATOR "
                                                 "AFTER MOVE - SetupStructureConquest" << std::endl;

                                    const Cell2D currCell(unit->GetRow0(), unit->GetCol0());
                                    const bool res = SetupStructureConquest(unit, currCell, end,
                                                                            player, basicOnDone);

                                    std::cout << "ScreenGame::ExecuteAIAction - CONQUER GENERATOR "
                                                 "AFTER MOVE - SetupStructureConquest - res: " << res << std::endl;

                                    if(!res)
                                        basicOnDone(false);
                                }
                                else
                                {
                                    std::cout << "ScreenGame::ExecuteAIAction - CONQUER GENERATOR "
                                                 "AFTER MOVE - MOVE FAILED" << std::endl;

                                    basicOnDone(false);
                                }
                            });
                    }
                }

                PrintAction(turnAI, action, done, player);
            }
            break;

            case AIA_UNIT_CONNECT_STRUCTURE:
            {
                auto unit = static_cast<Unit *>(action->ObjSrc);
                const Cell2D unitCell(unit->GetRow0(), unit->GetCol0());
                const Cell2D start = action->cellSrc;
                const GameMapCell & startGM = mGameMap->GetCell(start.row, start.col);

                // unit already on start or next to it
                if(unitCell == start || (startGM.owner == player &&
                   mGameMap->AreCellsOrthoAdjacent(unitCell, start)))
                {
                    std::cout << "ScreenGame::ExecuteAIAction - AI " << turnAI
                              << " - CONNECT STRUCTURE ADJACENT" << std::endl;

                    done = SetupConnectCellsAI(unit, basicOnDone);
                }
                // unit needs to move to the structure
                else
                {
                    std::cout << "ScreenGame::ExecuteAIAction - AI " << turnAI
                              << " - CONNECT STRUCTURE AFTER MOVE" << std::endl;

                    done = SetupUnitMove(unit, unitCell, start,
                        [this, unit, start, basicOnDone](bool successful)
                        {
                            if(successful)
                            {
                                const bool res = SetupConnectCellsAI(unit, basicOnDone);

                                std::cout << "ScreenGame::ExecuteAIAction - CONNECT STRUCTURE "
                                             "AFTER MOVE - SetupConnectCellsAI - res: " << res << std::endl;

                                if(!res)
                                    basicOnDone(false);
                            }
                            else
                            {
                                std::cout << "ScreenGame::ExecuteAIAction - CONNECT STRUCTURE "
                                             "AFTER MOVE - MOVE FAILED" << std::endl;

                                basicOnDone(false);
                            }
                        });
                }

                PrintAction(turnAI, action, done, player);
            }
            break;

            case AIA_NEW_UNIT:
            {
                auto a = static_cast<const ActionAINewUnit *>(action);

                done = SetupNewUnit(a->unitType, a->ObjSrc, ai->GetPlayer(), basicOnDone);

                PrintAction(turnAI, action, done, player);
            }
            break;

            case AIA_UNIT_COLLECT_BLOBS:
            {
                auto unit = static_cast<Unit *>(action->ObjSrc);

                const Cell2D cellUnit(unit->GetRow0(), unit->GetCol0());
                const Cell2D cellDest(action->ObjDst->GetRow0(), action->ObjDst->GetCol0());

                done = SetupUnitMove(unit, cellUnit, cellDest, basicOnDone);

                PrintAction(turnAI, action, done, player);
            }
            break;

            case AIA_UNIT_COLLECT_DIAMONDS:
            {
                auto unit = static_cast<Unit *>(action->ObjSrc);

                const Cell2D cellUnit(unit->GetRow0(), unit->GetCol0());
                const Cell2D cellDest(action->ObjDst->GetRow0(), action->ObjDst->GetCol0());

                done = SetupUnitMove(unit, cellUnit, cellDest, basicOnDone);

                PrintAction(turnAI, action, done, player);
            }
            break;

            case AIA_UNIT_COLLECT_LOOTBOX:
            {
                auto unit = static_cast<Unit *>(action->ObjSrc);

                const Cell2D cellUnit(unit->GetRow0(), unit->GetCol0());
                const Cell2D cellDest(action->ObjDst->GetRow0(), action->ObjDst->GetCol0());

                done = SetupUnitMove(unit, cellUnit, cellDest, basicOnDone);

                PrintAction(turnAI, action, done, player);
            }
            break;

            case AIA_UNIT_BUILD_STRUCTURE:
            {
                auto a = static_cast<const ActionAIBuildStructure *>(action);

                // decide where to build
                auto unit = static_cast<Unit *>(a->ObjSrc);
                unit->SetStructureToBuild(a->structType);

                const Cell2D cellUnit(unit->GetRow0(), unit->GetCol0());
                Cell2D target;

                if(FindWhereToBuildStructureAI(unit, target))
                {
                    if(mGameMap->AreCellsAdjacent(cellUnit, target))
                    {
                        std::cout << "ScreenGame::ExecuteAIAction - BUILD STRUCTURE ADJACENT - unit: "
                                  << cellUnit.row << "," << cellUnit.col
                                  << " - target: " << target.row << "," << target.col << std::endl;

                        done = SetupStructureBuilding(unit, target, player, basicOnDone);
                    }
                    else
                    {
                        const Cell2D moveTarget = mGameMap->GetAdjacentMoveTarget(cellUnit, target);

                        std::cout << "ScreenGame::ExecuteAIAction - BUILD STRUCTURE MOVE - unit: "
                                  << cellUnit.row << "," << cellUnit.col
                                  << " - target: " << target.row << "," << target.col
                                  << " - move target: " << moveTarget.row << "," << moveTarget.col
                                  << std::endl;

                        if(moveTarget.row != -1 && moveTarget.col != -1)
                        {
                            done = SetupUnitMove(unit, cellUnit, moveTarget,
                                [this, unit, target, player, basicOnDone](bool successful)
                                {
                                    if(successful)
                                    {
                                        const bool res = SetupStructureBuilding(unit, target, player, basicOnDone);

                                        std::cout << "ScreenGame::ExecuteAIAction - BUILD STRUCTURE "
                                                     "AFTER MOVE - SetupUnitMove - res: " << res << std::endl;

                                        if(!res)
                                        {
                                            basicOnDone(false);
                                            unit->ClearStructureToBuild();
                                        }
                                    }
                                    else
                                    {
                                        std::cout << "ScreenGame::ExecuteAIAction - BUILD STRUCTURE "
                                                     "AFTER MOVE - MOVE FAILED" << std::endl;

                                        basicOnDone(false);
                                        unit->ClearStructureToBuild();
                                    }
                                });
                        }
                        else
                            done = false;
                    }
                }
                else
                    done = false;

                if(!done)
                    unit->ClearStructureToBuild();

                PrintAction(turnAI, action, done, player);
            }
            break;

            case AIA_UNIT_ATTACK_TREES:
            {
                const auto unit = static_cast<Unit *>(action->ObjSrc);
                GameObject * target = action->ObjDst;

                if(unit->IsTargetAttackInRange(target))
                    done = SetupUnitAttack(unit, target, player, basicOnDone);
                else
                {
                    const Cell2D start(unit->GetRow0(), unit->GetCol0());
                    Cell2D dest;

                    if(mGameMap->FindAttackPosition(unit, action->ObjDst, dest))
                    {
                        done = SetupUnitMove(unit, start, dest,
                            [this, unit, target, player, basicOnDone](bool successful)
                            {
                                if(successful)
                                {
                                    const bool res = SetupUnitAttack(unit, target, player, basicOnDone);

                                    std::cout << "ScreenGame::ExecuteAIAction - ATTACK TREES "
                                                 "AFTER MOVE - SetupUnitMove - res: " << res << std::endl;

                                    if(!res)
                                        basicOnDone(false);
                                }
                                else
                                {
                                    std::cout << "ScreenGame::ExecuteAIAction - ATTACK TREES "
                                                 "AFTER MOVE - MOVE FAILED" << std::endl;

                                    basicOnDone(false);
                                }
                            });
                    }
                    else
                    {
                        std::cout << "ScreenGame::ExecuteAIAction - ATTACK TREES "
                                     "FindAttackPosition FAILED" << std::endl;
                        done = false;
                    }
                }

                PrintAction(turnAI, action, done, player);
            }
            break;

            default:
                std::cout << "ScreenGame::ExecuteAIAction - AI " << turnAI << " - UNKNOWN ACTION"
                          << action->GetTypeStr() << std::endl;
            break;
        }

        if(done)
            ai->RegisterActionInProgress(action);
    }
}

void ScreenGame::CancelObjectAction(GameObject * obj)
{
    auto it = mObjActions.begin();

    // search selected object in active actions
    while(it != mObjActions.end())
    {
        GameObjectAction & act = *it;

        if(act.obj == obj)
        {
            const GameObjectTypeId objType = act.obj->GetObjectType();
            const GameObjectActionType actType = act.type;

            // object is a Base
            if(objType == GameObject::TYPE_BASE)
            {
                // building a new unit
                if(actType == GameObjectActionType::BUILD_UNIT)
                    act.progressBar->DeleteLater();
            }
            // object is a Unit
            else if(act.obj->GetObjectCategory() == GameObject::CAT_UNIT)
            {
                if(actType == GameObjectActionType::MOVE)
                    mGameMap->AbortMove(obj);
                else if(actType == GameObjectActionType::CONQUER_CELL)
                    mGameMap->AbortCellConquest(obj);
                else if(actType == GameObjectActionType::BUILD_WALL)
                    mGameMap->AbortBuildWalls(obj);
                else if(actType == GameObjectActionType::CONQUER_STRUCTURE)
                {
                    mGameMap->AbortConquerStructure(act.target);
                    act.progressBar->DeleteLater();
                }
                else if(actType == GameObjectActionType::ATTACK)
                {
                    auto unit = static_cast<Unit *>(obj);
                    unit->ClearTargetAttack();
                }
                else if(actType == GameObjectActionType::HEAL)
                {
                    auto unit = static_cast<Unit *>(obj);
                    unit->ClearTargetHealing();
                }
            }
            // object is a Hospital
            else if(objType == GameObject::TYPE_HOSPITAL)
            {
                if(actType == GameObjectActionType::HEAL)
                {
                    auto hospital = static_cast<Hospital *>(obj);
                    hospital->ClearTargetHealing();
                }
            }

            mObjActions.erase(it);

            obj->SetCurrentAction(GameObjectActionType::IDLE);
            obj->SetActiveActionToDefault();

            // re-enable actions for local player
            if(obj->GetFaction() == mLocalPlayer->GetFaction())
            {
                ClearCellOverlays();

                mHUD->SetLocalActionsEnabled(true);

                // show current indicator for units
                if(obj->GetObjectCategory() == GameObject::CAT_UNIT)
                    ShowActiveIndicators(static_cast<Unit *>(act.obj), mCurrCell);
            }

            break;
        }
        else
            ++it;
    }
}

void ScreenGame::SetObjectActionDone(GameObject * obj, bool successful)
{
    // search selected object in active actions
    auto it = mObjActions.begin();

    while(it != mObjActions.end())
    {
        if(it->obj == obj)
        {
            std::cout << "ScreenGame::SetObjectActionDone - OBJ ACTIONS - obj ID: "
                      << obj->GetObjectId() << " - ACTION TYPE: "
                      << it->type << (successful ? " - OK" : " - FAIL") << std::endl;

            FinalizeObjectAction(*it, successful);

            // remove and destroy pending action
            mObjActions.erase(it);

            return ;
        }

        ++it;
    }

    // search selected object in actions to add for special cases when
    // action is completed immediately like AI quick actions
    it = mObjActionsToDo.begin();

    while(it != mObjActionsToDo.end())
    {
        if(it->obj == obj)
        {
            std::cout << "ScreenGame::SetObjectActionDone - OBJ ACTIONS TO DO - obj ID: "
                      << obj->GetObjectId() << " - ACTION TYPE: "
                      << it->type << (successful ? " - OK" : " - FAIL") << std::endl;

            FinalizeObjectAction(*it, successful);

            // remove and destroy pending action
            mObjActionsToDo.erase(it);

            return ;
        }

        ++it;
    }

    std::cout << "ScreenGame::SetObjectActionDone - ERROR can't find - obj ID: "
              << obj->GetObjectId() << std::endl;
}

void ScreenGame::FinalizeObjectAction(const GameObjectAction & action, bool successful)
{
    GameObject * obj = action.obj;

    // re-enable actions panel if obj is local
    if(obj->GetFaction() == mLocalPlayer->GetFaction())
        mHUD->SetLocalActionsEnabled(true);

    if(obj->IsFactionLocal())
        ClearCellOverlays();

    // reset object's active action to default
    obj->SetActiveActionToDefault();
    // reset current action to idle
    obj->SetCurrentAction(IDLE);

    // execute done callback
    action.onDone(successful);
}

void ScreenGame::UpdateGameEnd()
{
    // check if player has base for instant GAME OVER
    if(CheckGameOverForLocalPlayer())
    {
        mHUD->ShowDialogEndMission(false);
        return ;
    }

    // map already completed
    if(mMapCompleted)
        return ;

    // check goals
    unsigned int primaryGoals = 0;
    unsigned int completedPrimaryGoals = 0;

    for(MissionGoal & g : mMissionGoals)
    {
        if(g.IsPrimary())
            ++primaryGoals;

        const bool completed = CheckIfGoalCompleted(g);

        if(completed)
        {
            if(g.IsPrimary())
                ++completedPrimaryGoals;

            if(!g.IsRewardCollected())
                mHUD->ShowGoalCompletedIcon();
        }
    }

    if(completedPrimaryGoals == primaryGoals)
        mMapCompleted = true;
}

bool ScreenGame::CheckIfGoalCompleted(MissionGoal & g)
{
    if(g.IsCompleted())
        return true;

    switch(g.GetType())
    {
        case MG_COLLECT_BLOBS:
        {
            if(mResourcesGained[MR_BLOBS] < g.GetQuantity())
            {
                g.SetProgress(mResourcesGained[MR_BLOBS] * 100 / g.GetQuantity());

                return false;
            }
        }
        break;

        case MG_COLLECT_DIAMONDS:
        {
            if(mResourcesGained[MR_DIAMONDS] < g.GetQuantity())
            {
                g.SetProgress(mResourcesGained[MR_DIAMONDS] * 100 / g.GetQuantity());

                return false;
            }
        }
        break;

        case MG_DESTROY_ENEMY_BASE:
        {
            // check if destroyed all enemy bases
            for(Player * p : mAiPlayers)
            {
                if(p->HasStructure(GameObject::TYPE_BASE))
                return false;
            }
        }
        break;

        case MG_DESTROY_ALL_ENEMIES:
        {
            // check if destroyed all enemies
            for(Player * p : mAiPlayers)
            {
                if(p->GetNumObjects() > 0)
                    return false;
            }
        }
        break;

        case MG_GAIN_MONEY:
        {
            if(mResourcesGained[MR_MONEY] < g.GetQuantity())
            {
                g.SetProgress(mResourcesGained[MR_MONEY] * 100 / g.GetQuantity());

                return false;
            }
        }
        break;

        case MG_MINE_ENERGY:
        {
            if(mResourcesGained[MR_ENERGY] < g.GetQuantity())
            {
                g.SetProgress(mResourcesGained[MR_ENERGY] * 100 / g.GetQuantity());

                return false;
            }
        }
        break;

        case MG_MINE_MATERIAL:
        {
            if(mResourcesGained[MR_MATERIAL] < g.GetQuantity())
            {
                g.SetProgress(mResourcesGained[MR_MATERIAL] * 100 / g.GetQuantity());

                return false;
            }
        }
        break;

        case MG_RESIST_TIME:
        {
            // check elapsed time
            const unsigned int playedTime = GetPlayTimeInSec();

            if(playedTime < g.GetQuantity())
            {
                g.SetProgress(playedTime * 100 / g.GetQuantity());

                return false;
            }

            mHUD->HideMissionCountdown();
        }
        break;

        case MG_COMPLETE_TUTORIAL:
        {
            Game * game = GetGame();

            if(game->IsTutorialEnabled())
            {
                if(game->GetTutorialState(TUTORIAL_MISSION_INTRO) != TS_DONE)
                {
                    g.SetProgress(mTutMan->GetNumStepsDone() * 100 / mTutMan->GetNumStepsAtStart());
                    return false;
                }
            }
            else
                return false;
        }
        break;

        default:
            return false;
        break;
    }

    g.SetCompleted();

    return true;
}

void ScreenGame::UpdateGoalCompletedIcon()
{
    for(MissionGoal & g : mMissionGoals)
    {
        // there's still some reward to collect -> do not hide
        if(g.IsCompleted() && !g.IsRewardCollected())
            return;
    }

    mHUD->HideGoalCompletedIcon();
}

void ScreenGame::HandleGameOver()
{
    // decide winner
    unsigned int winner = 0;
    int topObjs = 0;

    for(unsigned int i = 0; i < mAiPlayers.size(); ++i)
    {
        Player * p = mAiPlayers[i];

        const int numObjs = p->GetNumObjects();

        if(numObjs > topObjs)
        {
            winner = i;
            topObjs = numObjs;
        }
    }

    // assign territory to winner
    AssignMapToFaction(mAiPlayers[winner]->GetFaction());
}

void ScreenGame::HandleGameWon()
{
    AssignMapToFaction(mLocalPlayer->GetFaction());
}

void ScreenGame::AssignMapToFaction(PlayerFaction faction)
{
    Game * game = GetGame();
    MapsRegistry * mapReg = game->GetMapsRegistry();

    const unsigned int territory = game->GetCurrentTerritory();
    const Planets planet = game->GetCurrentPlanet();

    mapReg->SetMapOccupier(planet, territory, faction);
    mapReg->SetMapStatus(planet, territory, TER_ST_OCCUPIED);

    if(faction == mLocalPlayer->GetFaction())
        mapReg->SetMapMissionCompleted(planet, territory);

    game->RequestNextActiveState(StateId::PLANET_MAP);
}

bool ScreenGame::CheckGameOverForLocalPlayer()
{
    // check if player still has base
    return !mLocalPlayer->HasStructure(GameObject::TYPE_BASE);
}

int ScreenGame::CellToIndex(const Cell2D & cell) const
{
    return cell.row * mIsoMap->GetNumCols() + cell.col;
}

bool ScreenGame::SetupNewUnit(GameObjectTypeId type, GameObject * gen, Player * player,
                              const std::function<void(bool)> & onDone)
{
    // check if create is possible
    if(!mGameMap->CanCreateUnit(type, gen, player))
        return false;

    Cell2D cell = mGameMap->GetNewUnitDestination(gen);

    if(-1 == cell.row || -1 == cell.col)
    {
        std::cerr << "GameMap::GetNewUnitDestination FAILED" << std::endl;
        return false;
    }

    // start create
    mGameMap->StartCreateUnit(type, gen, cell, player);

    // create and init progress bar
    // TODO get time from generator
    float timeBuild = TIME_NEW_UNIT;

    // special time for invisible AI
    if(!player->IsLocal() && !mGameMap->IsObjectVisibleToLocalPlayer(gen))
        timeBuild = TIME_AI_MIN;

    GameMapProgressBar * pb = mHUD->CreateProgressBarInCell(cell, timeBuild, player->GetFaction());

    pb->AddFunctionOnCompleted([this, cell, player, gen, type]
    {
        gen->ActionStepCompleted(BUILD_UNIT);
        gen->SetCurrentAction(GameObjectActionType::IDLE);

        mGameMap->CreateUnit(type, gen, cell, player);

        // add unit to map if cell is visible to local player
        if(mGameMap->IsCellVisibleToLocalPlayer(cell.row, cell.col))
        {
            const ObjectData & data = GetGame()->GetObjectsRegistry()->GetObjectData(type);

            const PlayerFaction faction = player->GetFaction();
            const auto type = static_cast<MiniMap::MiniMapElemType>(MiniMap::MME_FACTION1 + faction);
            MiniMap * mm = mHUD->GetMinimap();
            mm->AddElement(cell.row, cell.col, data.GetRows(), data.GetCols(), type, faction);
        }

        SetObjectActionCompleted(gen);
    });

    // store active action
    mObjActionsToDo.emplace_back(gen, GameObjectActionType::BUILD_UNIT, cell, pb, onDone);

    gen->SetActiveAction(GameObjectActionType::IDLE);
    gen->SetCurrentAction(GameObjectActionType::BUILD_UNIT);

    // disable actions panel (if action is done by local player)
    if(player->IsLocal())
        mHUD->SetLocalActionsEnabled(false);

    return true;
}

bool ScreenGame::SetupStructureConquest(Unit * unit, const Cell2D & start, const Cell2D & end,
                                        Player * player, const std::function<void(bool)> & onDone)
{
    // check if conquest is possible
    if(!mGameMap->CanConquerStructure(unit, end, player))
    {
        PlayLocalActionErrorSFX(player);
        return false;
    }

    const GameMapCell & gameCell = mGameMap->GetCell(end.row, end.col);
    GameObject * target = gameCell.objTop;

    // handle special case: TEMPLE
    if(player->IsLocal())
    {
        if(target->GetObjectType() == GameObject::TYPE_TEMPLE)
        {
            mHUD->ShowDialogExploreTemple(player, static_cast<Temple *>(target));
            return false;
        }
    }

    // start conquest
    mGameMap->StartConquerStructure(end, player);

    // create and init progress bar
    // TODO get time from unit
    float timeConquest = TIME_CONQ_RES_GEN;

    // special time for invisible AI
    if(!player->IsLocal() && !mGameMap->IsObjectVisibleToLocalPlayer(target))
        timeConquest = TIME_AI_MIN;

    GameMapProgressBar * pb = mHUD->CreateProgressBarInCell(start, timeConquest, player->GetFaction());

    pb->AddFunctionOnCompleted([this, start, end, player, unit]
    {
        mGameMap->ConquerStructure(end, player);

        unit->ActionStepCompleted(CONQUER_STRUCTURE);

        const GameMapCell & cellStruct = mGameMap->GetCell(end.row, end.col);
        const GameObject * objStruct = cellStruct.objTop;

        const PlayerFaction faction = player->GetFaction();
        const auto type = static_cast<MiniMap::MiniMapElemType>(MiniMap::MME_FACTION1 + faction);

        MiniMap * mm = mHUD->GetMinimap();
        mm->UpdateElement(objStruct->GetRow0(), objStruct->GetCol0(),
                          objStruct->GetRows(), objStruct->GetCols(), type, faction);

        // clear action data once the action is completed
        SetObjectActionCompleted(unit);
    });

    // store active action
    const GameMapCell & targetCell = mGameMap->GetCell(end.row, end.col);

    mObjActionsToDo.emplace_back(unit, targetCell.objTop, GameObjectActionType::CONQUER_STRUCTURE,
                             start, pb, onDone);

    unit->SetActiveAction(GameObjectActionType::IDLE);
    unit->SetCurrentAction(GameObjectActionType::CONQUER_STRUCTURE);

    // disable actions panel (if action is done by local player)
    if(player->IsLocal())
        mHUD->SetLocalActionsEnabled(false);

    return true;
}

bool ScreenGame::SetupStructureBuilding(Unit * unit, const Cell2D & cellTarget, Player * player,
                                        const std::function<void(bool)> & onDone)
{
    const GameObjectTypeId st = unit->GetStructureToBuild();

    // check if building is possible
    if(!mGameMap->CanBuildStructure(unit, cellTarget, player, st))
    {
        PlayLocalActionErrorSFX(player);
        return false;
    }

    mGameMap->StartBuildStructure(cellTarget, player, st);

    // create and init progress bar
    // TODO get time from unit
    GameMapProgressBar * pb = mHUD->CreateProgressBarInCell(cellTarget, TIME_CONQ_RES_GEN, player->GetFaction());

    pb->AddFunctionOnCompleted([this, unit, cellTarget, player, st]
    {
        mGameMap->BuildStructure(cellTarget, player, st);

        unit->ActionStepCompleted(BUILD_STRUCTURE);

        // add unit to map if cell is visible to local player
        const ObjectData & data = GetGame()->GetObjectsRegistry()->GetObjectData(st);

        const unsigned int rTL = cellTarget.row - data.GetRows() + 1;
        const unsigned int cTL = cellTarget.col - data.GetCols() + 1;

        if(mGameMap->IsAnyCellVisibleToLocalPlayer(rTL, cTL, cellTarget.row, cellTarget.col))
        {
            const PlayerFaction faction = player->GetFaction();
            const auto type = static_cast<MiniMap::MiniMapElemType>(MiniMap::MME_FACTION1 + faction);
            MiniMap * mm = mHUD->GetMinimap();
            mm->AddElement(cellTarget.row, cellTarget.col, data.GetRows(), data.GetCols(), type, faction);
        }

        // clear action data once the action is completed
        SetObjectActionCompleted(unit);
    });

    // store active action
    mObjActionsToDo.emplace_back(unit, GameObjectActionType::BUILD_STRUCTURE, cellTarget, pb, onDone);

    // disable actions panel and clear overlays (if action is done by local player)
    if(player->IsLocal())
    {
        mHUD->SetLocalActionsEnabled(false);

        ClearCellOverlays();
    }

    unit->SetActiveAction(GameObjectActionType::IDLE);
    unit->SetCurrentAction(GameObjectActionType::BUILD_STRUCTURE);
    unit->ClearStructureToBuild();

    return true;
}

bool ScreenGame::SetupUnitAttack(Unit * unit, GameObject * target, Player * player,
                                 const std::function<void(bool)> & onDone)
{
    const bool res = unit->SetTargetAttack(target);

    if(!res)
    {
        PlayLocalActionErrorSFX(player);
        return false;
    }

    unit->SetActiveAction(GameObjectActionType::IDLE);
    unit->SetCurrentAction(GameObjectActionType::ATTACK);

    // disable actions panel (if action is done by local player)
    if(player->IsLocal())
        mHUD->SetLocalActionsEnabled(false);

    mObjActionsToDo.emplace_back(unit, GameObjectActionType::ATTACK, onDone);

    return true;
}

bool ScreenGame::SetupHospitalHeal(Hospital * hospital, GameObject * target, Player * player,
                                   const std::function<void(bool)> & onDone)
{
    const bool res = hospital->SetTargetHealing(target);

    if(!res)
    {
        PlayLocalActionErrorSFX(player);
        return false;
    }

    hospital->SetActiveAction(GameObjectActionType::IDLE);
    hospital->SetCurrentAction(GameObjectActionType::HEAL);

    // disable actions panel (if action is done by local player)
    if(player->IsLocal())
        mHUD->SetLocalActionsEnabled(false);

    mObjActionsToDo.emplace_back(hospital, GameObjectActionType::HEAL, onDone);

    return true;
}

bool ScreenGame::SetupUnitHeal(Unit * unit, GameObject * target, Player * player,
                               const std::function<void(bool)> & onDone)
{
    const bool res = unit->SetTargetHealing(target);

    if(!res)
    {
        PlayLocalActionErrorSFX(player);
        return false;
    }

    unit->SetActiveAction(GameObjectActionType::IDLE);
    unit->SetCurrentAction(GameObjectActionType::HEAL);

    // disable actions panel (if action is done by local player)
    if(player->IsLocal())
        mHUD->SetLocalActionsEnabled(false);

    mObjActionsToDo.emplace_back(unit, GameObjectActionType::HEAL, onDone);

    return true;
}

bool ScreenGame::SetupUnitMove(Unit * unit, const Cell2D & start, const Cell2D & end,
                               const std::function<void(bool)> & onDone)
{
    const auto path = mPathfinder->MakePath(start.row, start.col, end.row, end.col,
                                            sgl::ai::Pathfinder::ALL_OPTIONS);

    const Player * player = GetGame()->GetPlayerByFaction(unit->GetFaction());

    // empty path -> exit
    if(path.empty())
    {
        PlayLocalActionErrorSFX(player);
        return false;
    }

    auto op = new ObjectPath(unit, mIsoMap, mGameMap, this);
    op->SetPathCells(path);

    if(mGameMap->MoveUnit(op))
    {
        // disable actions panel (if action is done by local player)
        if(unit->GetFaction() == mLocalPlayer->GetFaction())
        {
            mHUD->SetLocalActionsEnabled(false);

            ClearCellOverlays();
        }

        // store active action
        mObjActionsToDo.emplace_back(unit, GameObjectActionType::MOVE, onDone);

        unit->SetActiveAction(GameObjectActionType::IDLE);
        unit->SetCurrentAction(GameObjectActionType::MOVE);

        return true;
    }
    else
    {
        PlayLocalActionErrorSFX(player);
        return false;
    }
}

bool ScreenGame::SetupConnectCellsAI(Unit * unit, const std::function<void (bool)> & onDone)
{
    const int turnAI = mActivePlayerIdx - 1;

    // find closest linked cell
    const PlayerFaction faction = unit->GetFaction();
    const Player * player = GetGame()->GetPlayerByFaction(faction);
    const Cell2D start(unit->GetRow0(), unit->GetCol0());
    Cell2D target;

    if(!mGameMap->FindClosestLinkedCell(faction, start, target))
    {
        std::cout << "ScreenGame::SetupConnectCells - AI " << turnAI
                  << " - CONNECT STRUCTURE FAILED (can't find target)" << std::endl;
        return false;
    }

    auto cp = new ConquerPath(unit, mIsoMap, mGameMap, this);

    // special case: unit is already next to target
    if(mGameMap->AreCellsOrthoAdjacent(start, target))
    {
        const unsigned int startInd = (start.row * mGameMap->GetNumCols()) + start.col;
        const std::vector<unsigned int> path { startInd };

        cp->SetPathCells(path);

        std::cout << "ScreenGame::SetupConnectCells - AI " << turnAI
                  << " - CONNECT STRUCTURE - special case: unit ADJ target" << std::endl;
    }
    else
    {
        // if target cell has object try to find one next to it free
        if(mGameMap->HasObject(target.row, target.col))
        {
            const Cell2D oldTarget = target;

            target = mGameMap->GetOrthoAdjacentMoveTarget(start, target);

            // can't find an adjacent cell that's free
            if(-1 == target.row)
            {
                std::cout << "ScreenGame::SetupConnectCells - AI " << turnAI
                          << " - CONNECT STRUCTURE FAILED (GetOrthoAdjacentMoveTarget failed) - "
                             "start: " << start.row << "," << start.col
                          << " - target: " << oldTarget.row << "," << oldTarget.col << std::endl;

                return false;
            }
        }

        const auto path = mPathfinder->MakePath(start.row, start.col,
                                                target.row, target.col,
                                                sgl::ai::Pathfinder::INCLUDE_START);

        // can't find a path from start to target
        if(path.empty())
        {
            std::cout << "ScreenGame::SetupConnectCells - AI " << turnAI
                      << " - CONNECT STRUCTURE FAILED (no path) - "
                         "start: " << start.row << "," << start.col
                      << " - target: " << target.row << "," << target.col << std::endl;

            return false;
        }

        cp->SetPathCells(path);
    }

    // start conquest
    if(mGameMap->ConquerCells(cp))
    {
        // store active action
        mObjActionsToDo.emplace_back(unit, GameObjectActionType::CONQUER_CELL, onDone);

        std::cout << "ScreenGame::SetupConnectCells - AI " << turnAI
                  << " - CONNECT STRUCTURE - start: " << start.row << "," << start.col
                  << " - target: " << target.row << "," << target.col << std::endl;

        return true;
    }
    else
    {
        std::cout << "ScreenGame::SetupConnectCells - AI " << turnAI
                  << " - CONNECT STRUCTURE FAILED (ConquerCells failed) - "
                     "start: " << start.row << "," << start.col
                  << " - target: " << target.row << "," << target.col << std::endl;

        return false;
    }
}

bool ScreenGame::FindWhereToBuildStructureAI(Unit * unit, Cell2D & target)
{
    const PlayerFaction faction = unit->GetFaction();
    const GameObjectTypeId type = unit->GetStructureToBuild();
    const ObjectsDataRegistry * dataReg = GetGame()->GetObjectsRegistry();
    const ObjectData & data = dataReg->GetObjectData(type);
    const int rows = data.GetRows();
    const int cols = data.GetCols();

    Game * game = GetGame();

    // DECIDE WHERE TO LOOK FOR BUILDING AREA
    Player * player = game->GetPlayerByFaction(faction);

    Cell2D cellUnit(unit->GetRow0(), unit->GetCol0());
    Cell2D cellStart;

    std::vector<Structure *> structures;

    // build close to existing similar structure
    if(player->HasStructure(type))
    {
        structures = player->GetStructuresByType(type);

        std::cout << "ScreenGame::FindWhereToBuildStructureAI - search near similar structures "
                  << structures.size() << std::endl;
    }
    // no similar structure -> build close to base
    else
    {
        structures = player->GetStructuresByType(GameObject::TYPE_BASE);

        std::cout << "ScreenGame::FindWhereToBuildStructureAI - search near base" << std::endl;
    }

    // find similar structure which is closest to unit
    unsigned int bestInd = 0;
    int bestDist = mGameMap->GetNumRows() + mGameMap->GetNumCols();

    for(unsigned int s = 0; s < structures.size(); ++s)
    {
        Structure * structure = structures[s];

        // strucure made of multiple cells -> check 4 corners
        if(rows > 1 || cols > 1)
        {
            // top-left
            const Cell2D tl(structure->GetRow1(), structure->GetCol1());
            const int distTL = mGameMap->ApproxDistance(cellUnit, tl);

            if(distTL < bestDist)
            {
                cellStart = tl;
                bestDist = distTL;
            }

            // top-right
            const Cell2D tr(structure->GetRow1(), structure->GetCol0());
            const int distTR = mGameMap->ApproxDistance(cellUnit, tr);

            if(distTR < bestDist)
            {
                cellStart = tr;
                bestDist = distTR;
            }

            // bottom-left
            const Cell2D bl(structure->GetRow0(), structure->GetCol1());
            const int distBL = mGameMap->ApproxDistance(cellUnit, bl);

            if(distBL < bestDist)
            {
                cellStart = bl;
                bestDist = distBL;
            }
        }

        // bottom-right
        const Cell2D br(structure->GetRow0(), structure->GetCol0());
        const int distBR = mGameMap->ApproxDistance(cellUnit, br);

        if(distBR < bestDist)
        {
            cellStart = br;
            bestDist = distBR;
        }
    }

    std::cout << "ScreenGame::FindWhereToBuildStructureAI - unit cell: "
              << cellUnit.row << "," << cellUnit.col
              << " - closest structure corner: "
              << cellStart.row << "," << cellStart.col << std::endl;

    // find suitable spot close to cellStart
    const int maxRadius = mGameMap->GetNumRows() / 2;

    // first try to find an area big enough to have all sides free
    if(mGameMap->FindFreeArea(cellStart, rows + 2, cols + 2, maxRadius, target))
    {
        target.row -= 1;
        target.col -= 1;

        std::cout << "ScreenGame::FindWhereToBuildStructureAI - FOUND target area A - target: "
                  << target.row << "," << target.col << std::endl;

        return true;
    }
    else
        return mGameMap->FindFreeArea(cellStart, rows, cols, maxRadius, target);
;}

void ScreenGame::HandleUnitMoveOnMouseUp(Unit * unit, const Cell2D & clickCell)
{
    const Cell2D selCell(unit->GetRow0(), unit->GetCol0());

    const bool clickWalkable = mGameMap->IsCellWalkable(clickCell.row, clickCell.col);

    // destination is walkable -> try to generate a path and move
    if(clickWalkable)
    {
        SetupUnitMove(unit, selCell, clickCell);
        return ;
    }

    const GameMapCell & clickGameCell = mGameMap->GetCell(clickCell.row, clickCell.col);
    const GameObject * clickObj = clickGameCell.objTop;
    const bool clickVisited = clickObj && clickObj->IsVisited();

    // destination never visited (hence not visible as well) -> try to move close
    if(!clickVisited)
    {
        Cell2D target = mGameMap->GetCloseMoveTarget(selCell, clickCell);

        // failed to find a suitable target
        if(-1 == target.row || -1 == target.col)
            return ;

        SetupUnitMove(unit, selCell, target);

        return ;
    }

    // check if destination obj is visible
    const bool clickVisible = clickObj && clickObj->IsVisible();

    // visited, but not visible object -> exit
    if(!clickVisible)
        return ;

    // visible, but it can't be conquered -> exit
    if(!clickObj->CanBeConquered())
        return ;

    // unit can't conquer
    if(!unit->CanConquer())
        return ;

    // object is adjacent -> try to interact
    if(mGameMap->AreObjectsAdjacent(unit, clickObj))
        SetupStructureConquest(unit, selCell, clickCell, mLocalPlayer);
    // object is far -> move close and then try to interact
    else
    {
        Cell2D target = mGameMap->GetAdjacentMoveTarget(selCell, clickObj);

        // failed to find a suitable target
        if(-1 == target.row || -1 == target.col)
            return ;

        SetupUnitMove(unit, selCell, target,
            [this, unit, clickCell](bool successful)
            {
                if(successful)
                {
                    const Cell2D currCell(unit->GetRow0(), unit->GetCol0());
                    SetupStructureConquest(unit, currCell, clickCell, mLocalPlayer);
                }
            });
    }
}

void ScreenGame::HandleUnitBuildStructureOnMouseUp(Unit * unit, const Cell2D & clickCell)
{
    const int clickInd = clickCell.row * mGameMap->GetNumCols() + clickCell.col;

    // destination is visible and walkable
    if(mLocalPlayer->IsCellVisible(clickInd) && mGameMap->IsCellWalkable(clickCell.row, clickCell.col))
    {
        const GameMapCell * gmc = unit->GetCell();
        const Cell2D cellUnit(gmc->row, gmc->col);

        const GameObjectTypeId st = unit->GetStructureToBuild();
        const ObjectsDataRegistry * dataReg = GetGame()->GetObjectsRegistry();
        const ObjectData & objData = dataReg->GetObjectData(st);

        // if unit is next to any target cell -> try to build
        const int indRows = objData.GetRows();
        const int indCols = objData.GetCols();
        const int r0 = clickCell.row >= indRows ? 1 + clickCell.row - indRows : 0;
        const int c0 = clickCell.col >= indCols ? 1 + clickCell.col - indCols : 0;

        bool next2Target = false;

        for(int r = r0; r <= clickCell.row; ++r)
        {
            for(int c = c0; c <= clickCell.col; ++c)
            {
                next2Target = mGameMap->AreCellsAdjacent(cellUnit, {r, c});

                if(next2Target)
                    break;
            }

            if(next2Target)
                break;
        }

        if(next2Target)
            SetupStructureBuilding(unit, clickCell, mLocalPlayer);
        // unit is far -> move close then try to build
        else
        {
            Cell2D target = mGameMap->GetAdjacentMoveTarget(cellUnit, {r0, c0}, clickCell);

            // failed to find a suitable target
            if(-1 == target.row || -1 == target.col)
                return ;

            // add temporary indicator for tower
            mTempStructIndicator = new StructureIndicator(objData, unit->GetFaction());

            IsoLayer * layer = mIsoMap->GetLayer(MapLayers::OBJECTS2);
            layer->AddObject(mTempStructIndicator, clickCell.row, clickCell.col);

            // move
            SetupUnitMove(unit, cellUnit, target,
                [this, unit, clickCell](bool successful)
            {
                if(successful)
                {
                    const Cell2D currCell(unit->GetRow0(), unit->GetCol0());
                    SetupStructureBuilding(unit, clickCell, mLocalPlayer);
                }

                ClearTempStructIndicator();
            });
        }
    }
}

void ScreenGame::HandleUnitBuildWallOnMouseUp(Unit * unit, const Cell2D & clickCell)
{
    const Cell2D unitCell(unit->GetRow0(), unit->GetCol0());
    const int clickInd = clickCell.row * mGameMap->GetNumCols() + clickCell.col;
    const bool diffClick = unitCell != clickCell;

    const Player * player = GetGame()->GetPlayerByFaction(unit->GetFaction());

    // not clicking on unit cell, destination is visible and walkable
    if(!diffClick || !mLocalPlayer->IsCellVisible(clickInd) ||
       !mGameMap->IsCellWalkable(clickCell.row, clickCell.col))
    {
        PlayLocalActionErrorSFX(player);
        return ;
    }

    // default is starting pathfinding from unit position
    sgl::ai::Pathfinder::PathOptions po = sgl::ai::Pathfinder::INCLUDE_START;
    unsigned int startR = unitCell.row;
    unsigned int startC = unitCell.col;

    if(!mWallPath.empty())
    {
        // reclicked on same cell of last path -> double click -> finalize path
        if(mWallPath.back() == clickInd)
        {
            const unsigned int minPathSize = 2;

            if(mWallPath.size() > minPathSize)
            {
                auto onFail = [this]
                {
                    mWallPath.clear();
                    ClearCellOverlays();
                };

                auto onDone = [this, unit, onFail](bool successful)
                {
                    if(successful)
                        StartUnitBuildWall(unit);
                    else
                        onFail();
                };

                const unsigned int prevInd = mWallPath[mWallPath.size() - minPathSize];
                const int endR = prevInd / mIsoMap->GetNumCols();
                const int endC = prevInd % mIsoMap->GetNumCols();

                const auto pathMov = mPathfinder->MakePath(unitCell.row, unitCell.col,
                                                           endR, endC);

                // empty path -> exit
                if(pathMov.empty())
                {
                    onFail();
                    PlayLocalActionErrorSFX(player);
                    return ;
                }

                auto op = new ObjectPath(unit, mIsoMap, mGameMap, this);
                op->SetPathCells(pathMov);

                const bool res = mGameMap->MoveUnit(op);

                // movement failed
                if(!res)
                {
                    onFail();
                    PlayLocalActionErrorSFX(player);
                    return;
                }

                // disable actions panel (if action is done by local player)
                mHUD->SetLocalActionsEnabled(false);

                // store active action
                mObjActionsToDo.emplace_back(unit, GameObjectActionType::MOVE, onDone);

                unit->SetActiveAction(GameObjectActionType::IDLE);
                unit->SetCurrentAction(GameObjectActionType::MOVE);
            }
            // only 1 block of wall -> no movement, start building
            else
            {
                if(!StartUnitBuildWall(unit))
                    PlayLocalActionErrorSFX(player);
            }

            return ;
        }
        // continue wall planning
        else
        {
            po = sgl::ai::Pathfinder::NO_OPTION;

            const unsigned int pathInd = mWallPath.back();
            startR = pathInd / mIsoMap->GetNumCols();
            startC = pathInd % mIsoMap->GetNumCols();
        }
    }

    const auto path = mPathfinder->MakePath(startR, startC, clickCell.row, clickCell.col, po);

    // empty path -> nothing to do
    if(path.empty())
    {
        PlayLocalActionErrorSFX(player);
        return ;
    }

    mWallPath.reserve(mWallPath.size() + path.size());
    mWallPath.insert(mWallPath.end(), path.begin(), path.end());
}

void ScreenGame::HandleSelectionClick(sgl::core::MouseButtonEvent & event)
{
    GameObject * currSel = mLocalPlayer->GetSelectedObject();

    // do not allow any selecting/deselection when an action is in progress
    if(currSel != nullptr && currSel->GetCurrentAction() != IDLE)
        return ;

    const sgl::graphic::Camera * cam = mCamController->GetCamera();
    const int worldX = cam->GetScreenToWorldX(event.GetX());
    const int worldY = cam->GetScreenToWorldY(event.GetY());
    const Cell2D clickCell = mIsoMap->CellFromWorldPoint(worldX, worldY);

    // clicked outside the map -> clear current selection
    if(!mIsoMap->IsCellInside(clickCell))
    {
        ClearSelection(mLocalPlayer);
        return ;
    }

    // get clicked object, if any
    const GameMapCell & clickGameCell = mGameMap->GetCell(clickCell.row, clickCell.col);
    GameObject * clickObj = clickGameCell.objTop ? clickGameCell.objTop : clickGameCell.objBottom;
    const bool isClickObjOwn = clickObj != nullptr && clickObj->GetFaction() == mLocalPlayer->GetFaction();

    // clicked non-own or no object -> nothing to do
    if(!isClickObjOwn)
        return ;

    // clicked selected object -> deselect it
    if(clickObj == currSel)
    {
        ClearSelection(mLocalPlayer);
        return ;
    }

    // normal selection -> clear current selection and select clicked object
    ClearSelection(mLocalPlayer);
    SelectObject(clickObj, mLocalPlayer);
}

void ScreenGame::HandleActionClick(sgl::core::MouseButtonEvent & event)
{
    // no object selected -> nothing to do
    if(!mLocalPlayer->HasSelectedObject())
        return ;

    const sgl::graphic::Camera * cam = mCamController->GetCamera();
    const int worldX = cam->GetScreenToWorldX(event.GetX());
    const int worldY = cam->GetScreenToWorldY(event.GetY());

    const Cell2D clickCell = mIsoMap->CellFromWorldPoint(worldX, worldY);

    // clicked outside the map -> nothing to do
    if(!mIsoMap->IsCellInside(clickCell))
        return ;

    GameObject * selObj = mLocalPlayer->GetSelectedObject();
    const Cell2D selCell(selObj->GetRow0(), selObj->GetCol0());

    // check if there's a lower object when top is empty
    const GameMapCell & clickGameCell = mGameMap->GetCell(clickCell.row, clickCell.col);
    GameObject * clickObj = clickGameCell.objTop ? clickGameCell.objTop : clickGameCell.objBottom;

    // selected object is a unit
    if(selObj->GetObjectCategory() == GameObject::CAT_UNIT)
    {
        auto selUnit = static_cast<Unit *>(selObj);

        const GameObjectActionType action = selUnit->GetActiveAction();

        if(action == GameObjectActionType::MOVE)
        {
            const bool diffClick = selCell != clickCell;

            // try to move only if clicked on a different cell
            if(diffClick)
                HandleUnitMoveOnMouseUp(selUnit, clickCell);
        }
        else if(action == GameObjectActionType::ATTACK)
            SetupUnitAttack(selUnit, clickObj, mLocalPlayer);
        else if(action == GameObjectActionType::HEAL)
            SetupUnitHeal(selUnit, clickObj, mLocalPlayer);
        else if(action == GameObjectActionType::CONQUER_CELL)
        {
            const Player * player = GetGame()->GetPlayerByFaction(selUnit->GetFaction());
            const int clickInd = clickCell.row * mGameMap->GetNumCols() + clickCell.col;

            // destination is visible and walkable or conquering unit cell
            if(mLocalPlayer->IsCellVisible(clickInd) &&
               (mGameMap->IsCellWalkable(clickCell.row, clickCell.col) || clickCell == selCell))
            {
                // init start for empty path
                sgl::ai::Pathfinder::PathOptions po = sgl::ai::Pathfinder::INCLUDE_START;
                unsigned int startR = selCell.row;
                unsigned int startC = selCell.col;

                // continue building path
                if(!mConquestPath.empty())
                {
                    // reclicked on same cell of last path -> double click -> finalize path
                    if(mConquestPath.back() == clickInd)
                    {
                        // start conquest
                        auto cp = new ConquerPath(selUnit, mIsoMap, mGameMap, this);
                        cp->SetPathCells(mConquestPath);

                        if(mGameMap->ConquerCells(cp))
                        {
                            mConquestPath.clear();

                            ClearCellOverlays();

                            // store active action
                            mObjActionsToDo.emplace_back(selUnit, action, [](bool){});

                            // disable action buttons
                            mHUD->SetLocalActionsEnabled(false);

                            selUnit->SetActiveAction(GameObjectActionType::IDLE);
                            selUnit->SetCurrentAction(GameObjectActionType::CONQUER_CELL);
                        }
                        else
                            PlayLocalActionErrorSFX(player);

                        return ;
                    }
                    // continue pathfinfing from latest click
                    else
                    {
                        po = sgl::ai::Pathfinder::NO_OPTION;

                        const unsigned int pathInd = mConquestPath.back();
                        startR = pathInd / mIsoMap->GetNumCols();
                        startC = pathInd % mIsoMap->GetNumCols();
                    }
                }

                const auto path = mPathfinder->MakePath(startR, startC, clickCell.row,
                                                        clickCell.col, po);

                // empty path -> nothing to do
                if(path.empty())
                {
                    PlayLocalActionErrorSFX(player);
                    return ;
                }

                mConquestPath.reserve(mConquestPath.size() + path.size());
                mConquestPath.insert(mConquestPath.end(), path.begin(), path.end());
            }
        }
        else if(action == GameObjectActionType::BUILD_WALL)
            HandleUnitBuildWallOnMouseUp(selUnit, clickCell);
        else if (action == GameObjectActionType::BUILD_STRUCTURE)
            HandleUnitBuildStructureOnMouseUp(selUnit, clickCell);
    }
    else if(selObj->GetObjectType() == GameObject::TYPE_HOSPITAL)
    {
        auto selHospital = static_cast<Hospital *>(selObj);

        const GameObjectActionType action = selHospital->GetActiveAction();

        if(action == GameObjectActionType::HEAL)
            SetupHospitalHeal(selHospital, clickObj, mLocalPlayer);
    }
}

bool ScreenGame::StartUnitBuildWall(Unit * unit)
{
    // setup build
    auto wbp = new WallBuildPath(unit, mIsoMap, mGameMap, this);
    wbp->SetPathCells(mWallPath);
    // NOTE only level 0 for now
    wbp->SetWallLevel(0);

    if(mGameMap->BuildWalls(wbp))
    {
        // action done by local player
        if(unit->GetFaction() == mLocalPlayer->GetFaction())
        {
            mHUD->SetLocalActionsEnabled(false);

            ClearCellOverlays();
        }

        mWallPath.clear();

        // store active action
        mObjActionsToDo.emplace_back(unit, GameObjectActionType::BUILD_WALL, [](bool){});

        unit->SetActiveAction(GameObjectActionType::IDLE);
        unit->SetCurrentAction(GameObjectActionType::BUILD_WALL);

        return true;
    }
    else
    {
        ClearCellOverlays();
        return false;
    }
}

void ScreenGame::ShowActiveIndicators(Unit * unit, const Cell2D & cell)
{
    const GameObjectActionType action = unit->GetActiveAction();

    if(action == GameObjectActionType::MOVE)
        ShowMoveIndicator(unit, cell);
    else if(action == GameObjectActionType::CONQUER_CELL)
        ShowConquestIndicator(unit, cell);
    else if(action == GameObjectActionType::BUILD_WALL)
        ShowBuildWallIndicator(unit, cell);
    else if(action == GameObjectActionType::BUILD_STRUCTURE)
        ShowBuildStructureIndicator(unit, cell);
}

void ScreenGame::ShowAttackIndicators(const GameObject * obj, int range)
{
    IsoLayer * layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS3);

    const int rows = mIsoMap->GetNumRows();
    const int cols = mIsoMap->GetNumCols();
    const int r0 = obj->GetRow0();
    const int c0 = obj->GetCol0();
    const int rowTL = obj->GetRow1() - range > 0 ? obj->GetRow1() - range : 0;
    const int colTL = obj->GetCol1() - range > 0 ? obj->GetCol1() - range : 0;
    const int rowBR = r0 + range < rows ? r0 + range : rows - 1;
    const int colBR = c0 + range < cols ? c0 + range : cols - 1;

    const int neededInd = (rowBR - rowTL + 1) * (colBR - colTL + 1);
    const int existingInd = mAttIndicators.size();
    const int missingInd = neededInd - existingInd;

    // create missing indicators
    if(missingInd > 0)
    {
        for(int i = 0; i < missingInd; ++i)
            mAttIndicators.push_back(new AttackRangeIndicator);
    }

    // init needed indicators
    const PlayerFaction faction = obj->GetFaction();

    for(int i = 0; i < neededInd; ++i)
    {
        mAttIndicators[i]->SetVisible(true);
        mAttIndicators[i]->SetFaction(faction);
    }

    // hide other indicators
    const int existingInd2 = mAttIndicators.size();

    for(int i = neededInd; i < existingInd2; ++i)
        mAttIndicators[i]->SetVisible(false);

    int ind = 0;

    for(int r = rowTL; r <= rowBR; ++r)
    {
        for(int c = colTL; c <= colBR; ++c)
        {
            if(r != r0 || c != c0)
                layer->AddObject(mAttIndicators[ind++], r, c);
        }
    }
}

void ScreenGame::ShowBuildStructureIndicator(Unit * unit, const Cell2D & currCell)
{
    IsoLayer * layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS4);

    // clear any current indicator
    layer->ClearObjects();

    // check if mouse is inside map
    const bool currInside = mIsoMap->IsCellInside(currCell);

    if(!currInside)
        return ;

    // check if unit is next to destination or if there's any walkable cell surrounding destination
    const int nextDist = 1;

    if((std::abs(unit->GetRow0() - currCell.row) > nextDist ||
         std::abs(unit->GetCol0() - currCell.col) > nextDist) &&
        !mGameMap->IsAnyNeighborCellWalkable(currCell.row, currCell.col))
        return ;

    // check if there's a path between unit and destination
    const auto path = mPathfinder->MakePath(unit->GetRow0(), unit->GetCol0(),
                                            currCell.row, currCell.col,
                                            sgl::ai::Pathfinder::ALL_OPTIONS);

    if(path.empty())
        layer->ClearObjects();

    // get an indicator
    const GameObjectTypeId st = unit->GetStructureToBuild();

    StructureIndicator * ind = nullptr;
    auto it = mStructIndicators.find(st);

    if(it != mStructIndicators.end())
        ind = it->second;
    else
    {
        const ObjectsDataRegistry * dataReg = GetGame()->GetObjectsRegistry();
        ind = new StructureIndicator(dataReg->GetObjectData(st), unit->GetFaction());
        mStructIndicators.emplace(st, ind);
    }

    // add indicator to layer
    layer->AddObject(ind, currCell.row, currCell.col);

    // set visibility
    const int indRows = ind->GetRows();
    const int indCols = ind->GetCols();
    const int r0 = currCell.row >= indRows ? 1 + currCell.row - indRows : 0;
    const int c0 = currCell.col >= indCols ? 1 + currCell.col - indCols : 0;

    bool showIndicator = true;

    for(int r = r0; r <= currCell.row; ++r)
    {
        const int idx0 = r * mGameMap->GetNumCols();

        for(int c = c0; c <= currCell.col; ++c)
        {
            const int idx = idx0 + c;

            showIndicator = mLocalPlayer->IsCellVisible(idx) && mGameMap->IsCellWalkable(idx);

            if(!showIndicator)
                break;
        }

        if(!showIndicator)
            break;
    }

    layer->SetObjectVisible(ind, showIndicator);
}

void ScreenGame::ShowConquestIndicator(Unit * unit, const Cell2D & dest)
{
    IsoLayer * layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS2);

    // first clear all objects from the layer
    layer->ClearObjects();

    const bool currInside = mIsoMap->IsCellInside(dest);

    // mouse outside the map
    if(!currInside)
        return ;

    const int currInd = dest.row * mGameMap->GetNumCols() + dest.col;

    const bool currVisible = mLocalPlayer->IsCellVisible(currInd);
    const bool currWalkable = mGameMap->IsCellWalkable(currInd);
    const bool currIsUnitCell = dest.row == unit->GetRow0() && dest.col == unit->GetCol0();

    const bool canConquer = currVisible && (currWalkable || currIsUnitCell);

    if(!canConquer)
        return ;

    sgl::ai::Pathfinder::PathOptions po;
    unsigned int startR;
    unsigned int startC;

    // start pathfinding from unit position
    if(mConquestPath.empty())
    {
        po = sgl::ai::Pathfinder::INCLUDE_START;

        startR = unit->GetRow0();
        startC = unit->GetCol0();
    }
    // continue pathfinfing from latest click
    else
    {
        po = sgl::ai::Pathfinder::NO_OPTION;

        const unsigned int pathInd = mConquestPath.back();
        startR = pathInd / mIsoMap->GetNumCols();
        startC = pathInd % mIsoMap->GetNumCols();
    }

    // show path cost when destination is visible
    const auto path = mPathfinder->MakePath(startR, startC, dest.row, dest.col, po);

    // this should never happen, but just in case
    if(path.empty() && mConquestPath.empty())
        return ;

    std::vector<unsigned int> totPath;
    totPath.reserve(mConquestPath.size() + path.size());

    totPath = mConquestPath;
    totPath.insert(totPath.end(), path.begin(), path.end());

    const unsigned int lastIdx = totPath.size() - 1;

    const PlayerFaction faction = mLocalPlayer->GetFaction();

    for(unsigned int i = 0; i < totPath.size(); ++i)
    {
        ConquestIndicator * ind = nullptr;

        if(i < mConquestIndicators.size())
            ind = mConquestIndicators[i];
        else
        {
            ind = new ConquestIndicator;
            mConquestIndicators.emplace_back(ind);
        }

        // add indicator to layer
        const unsigned int pathInd = totPath[i];
        const unsigned int indRow = pathInd / mIsoMap->GetNumCols();
        const unsigned int indCol = pathInd % mIsoMap->GetNumCols();

        layer->AddObject(ind, indRow, indCol);

        ind->SetFaction(faction);
        ind->ShowCost(i == lastIdx);
    }

    ConquerPath cp(unit, mIsoMap, mGameMap, this);
    cp.SetPathCells(totPath);

    mConquestIndicators[lastIdx]->SetCost(cp.GetPathCost());
}

void ScreenGame::ShowBuildWallIndicator(Unit * unit, const Cell2D & dest)
{
    IsoLayer * layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS2);

    // first clear all objects from the layer
    layer->ClearObjects();

    const bool currInside = mIsoMap->IsCellInside(dest);

    // mouse outside the map
    if(!currInside)
        return ;

    const int currInd = dest.row * mGameMap->GetNumCols() + dest.col;

    const bool currVisible = mLocalPlayer->IsCellVisible(currInd);
    const bool currWalkable = mGameMap->IsCellWalkable(currInd);
    const bool currIsUnitCell = dest.row == unit->GetRow0() && dest.col == unit->GetCol0();

    const bool canBuild = currVisible && (currWalkable || currIsUnitCell);

    if(!canBuild)
        return ;

    sgl::ai::Pathfinder::PathOptions po;
    unsigned int startR;
    unsigned int startC;

    // start pathfinding from unit position
    if(mWallPath.empty())
    {
        po = sgl::ai::Pathfinder::INCLUDE_START;

        startR = unit->GetRow0();
        startC = unit->GetCol0();
    }
    // continue pathfinfing from latest click
    else
    {
        po = sgl::ai::Pathfinder::NO_OPTION;

        const unsigned int pathInd = mWallPath.back();
        startR = pathInd / mIsoMap->GetNumCols();
        startC = pathInd % mIsoMap->GetNumCols();
    }

    // show path cost when destination is visible
    const auto path = mPathfinder->MakePath(startR, startC, dest.row, dest.col, po);

    // this should not happen
    if(path.size() < 2 && mWallPath.empty())
        return ;

    std::vector<unsigned int> totPath;
    totPath.reserve(mWallPath.size() + path.size());

    totPath = mWallPath;
    totPath.insert(totPath.end(), path.begin(), path.end());

    const unsigned int lastIdx = totPath.size() - 1;

    const PlayerFaction faction = mLocalPlayer->GetFaction();

    std::vector<Cell2D> cellsPath;
    cellsPath.reserve(totPath.size());

    // store coordinates of start cell
    const unsigned int pathInd0 = totPath[0];
    const unsigned int indRow0 = pathInd0 / mIsoMap->GetNumCols();
    const unsigned int indCol0 = pathInd0 % mIsoMap->GetNumCols();
    cellsPath.emplace_back(indRow0, indCol0);

    for(unsigned int i = 0; i < lastIdx; ++i)
    {
        WallIndicator * ind = nullptr;

        if(i < mWallIndicators.size())
            ind = mWallIndicators[i];
        else
        {
            ind = new WallIndicator;
            mWallIndicators.emplace_back(ind);
        }

        // add indicator to layer - skip path[0] as that's start
        const unsigned int pathInd = totPath[i + 1];
        const unsigned int indRow = pathInd / mIsoMap->GetNumCols();
        const unsigned int indCol = pathInd % mIsoMap->GetNumCols();
        cellsPath.emplace_back(indRow, indCol);

        layer->AddObject(ind, indRow, indCol);

        ind->SetFaction(faction);
        ind->ShowCost(false);
    }

    // -- set directions and costs --
    const unsigned int lastIndicator = lastIdx - 1;

    WallBuildPath wbp(unit, mIsoMap, mGameMap, this);
    wbp.SetPathCells(totPath);
    wbp.SetIndicatorsType(cellsPath, mWallIndicators);

    mWallIndicators[lastIndicator]->SetCost(wbp.GetEnergyCost(), wbp.GetMateriaCost());
}

void ScreenGame::ShowHealingIndicators(const GameObject * obj, int range)
{
    IsoLayer * layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS3);

    const int rows = mIsoMap->GetNumRows();
    const int cols = mIsoMap->GetNumCols();
    const int r0 = obj->GetRow0();
    const int c0 = obj->GetCol0();
    const int rowTL = obj->GetRow1() - range > 0 ? obj->GetRow1() - range : 0;
    const int colTL = obj->GetCol1() - range > 0 ? obj->GetCol1() - range : 0;
    const int rowBR = r0 + range < rows ? r0 + range : rows - 1;
    const int colBR = c0 + range < cols ? c0 + range : cols - 1;

    const int neededInd = (rowBR - rowTL + 1) * (colBR - colTL + 1);
    const int existingInd = mHealIndicators.size();
    const int missingInd = neededInd - existingInd;

    // create missing indicators
    if(missingInd > 0)
    {
        for(int i = 0; i < missingInd; ++i)
            mHealIndicators.push_back(new HealingRangeIndicator);
    }

    // init needed indicators
    const PlayerFaction faction = obj->GetFaction();

    for(int i = 0; i < neededInd; ++i)
    {
        mHealIndicators[i]->SetVisible(true);
        mHealIndicators[i]->SetFaction(faction);
    }

    // hide other indicators
    const int existingInd2 = mHealIndicators.size();

    for(int i = neededInd; i < existingInd2; ++i)
        mHealIndicators[i]->SetVisible(false);

    int ind = 0;

    for(int r = rowTL; r <= rowBR; ++r)
    {
        for(int c = colTL; c <= colBR; ++c)
        {
            if(r != r0 || c != c0)
                layer->AddObject(mHealIndicators[ind++], r, c);
        }
    }
}

void ScreenGame::ShowMoveIndicator(GameObject * obj, const Cell2D & dest)
{
    IsoLayer * layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS2);

    // mouse outside the map
    if(!mIsoMap->IsCellInside(dest))
    {
        // hide the indicator, if any
        if(mMoveInd != nullptr)
            layer->SetObjectVisible(mMoveInd, false);

        return ;
    }

    // move indicator already created -> move it and continue
    if(mMoveInd != nullptr)
        layer->MoveObject(mMoveInd->GetRow(), mMoveInd->GetCol(), dest.row, dest.col);
    // create new move indicator
    else
    {
        mMoveInd = new MoveIndicator;
        layer->AddObject(mMoveInd, dest.row, dest.col);
    }

    const int destInd = dest.row * mGameMap->GetNumCols() + dest.col;

    const bool destVisible = mLocalPlayer->IsCellVisible(destInd);
    const bool destVisited = mGameMap->IsCellObjectVisited(destInd);
    const bool destWalkable = mGameMap->IsCellWalkable(destInd);

    const bool showIndicator = (!destVisible && !destVisited) || destWalkable;

    layer->SetObjectVisible(mMoveInd, showIndicator);

    // stop here if not showing indicator
    if(!showIndicator)
        return ;

    if(destVisible)
    {
        // set indicator type
        mMoveInd->SetIndicatorType(MoveIndicator::NORMAL);

        // show path cost when destination is visible
        const auto path = mPathfinder->MakePath(obj->GetRow0(), obj->GetCol0(),
                                                dest.row, dest.col,
                                                sgl::ai::Pathfinder::ALL_OPTIONS);

        ObjectPath op(obj, mIsoMap, mGameMap, this);
        op.SetPathCells(path);

        mMoveInd->SetCost(op.GetPathCost());
    }
    // not visible destination
    else
    {
        // set indicator type
        mMoveInd->SetIndicatorType(MoveIndicator::NO_VIS_CELL);

        // hide cost when destination is not visible
        mMoveInd->SetCostUnknown();
    }
}

void ScreenGame::ClearCellOverlays()
{
    IsoLayer * layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS2);
    layer->ClearObjects();

    layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS3);
    layer->ClearObjects();

    layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS4);
    layer->ClearObjects();

    // delete move indicator
    delete mMoveInd;
    mMoveInd = nullptr;
}

void ScreenGame::ClearTempStructIndicator()
{
    if(mTempStructIndicator)
    {
        IsoLayer * layer = mIsoMap->GetLayer(MapLayers::OBJECTS2);
        layer->ClearObject(mTempStructIndicator);

        delete mTempStructIndicator;
        mTempStructIndicator = nullptr;
    }
}

void ScreenGame::CenterCameraOverPlayerBase()
{
    const Base * b = mLocalPlayer->GetBase();

    if(nullptr == b)
        return ;

    const sgl::core::Pointd2D pos = mIsoMap->GetCellPosition(b->GetRow0(), b->GetCol0());
    const int cX = pos.x + mIsoMap->GetTileWidth() * 0.5f;
    const int cY = pos.y + mIsoMap->GetTileHeight() * 0.5f;

    mCamController->CenterCameraToPoint(cX, cY);
}

void ScreenGame::UpdateCurrentCell()
{
    // NOTE this could be moved to cam->SetFunctionOnMove if things get more complicated
    // for now I am calling it manually to minimize calls
    const sgl::graphic::Camera * cam = mCamController->GetCamera();
    const int worldX = cam->GetScreenToWorldX(mMousePos.x);
    const int worldY = cam->GetScreenToWorldY(mMousePos.y);

    const Cell2D cell = mIsoMap->CellFromWorldPoint(worldX, worldY);

    if(cell == mCurrCell)
        return ;

    mCurrCell = cell;

    // react to change of cell like if mouse was moved
    GameObject * sel = mLocalPlayer->GetSelectedObject();

    if(sel != nullptr && sel->GetObjectCategory() == GameObject::CAT_UNIT)
        ShowActiveIndicators(static_cast<Unit *>(sel), cell);
}

void ScreenGame::SetMissionRewards()
{
    // init support data
    mResourcesGained.resize(NUM_MISSION_REWARDS, 0);

    // set rewards for all goals
    for(MissionGoal & g : mMissionGoals)
    {
        switch (g.GetType())
        {
            case MG_COLLECT_BLOBS:
            {
                const int divDiamonds = 10;
                const int diamonds = g.GetQuantity() / divDiamonds;
                g.SetRewardByType(MR_DIAMONDS, diamonds);

                const int multMoney = 5;
                const int money = g.GetQuantity() * multMoney;
                g.SetRewardByType(MR_MONEY, money);
            }
            break;

            case MG_COLLECT_DIAMONDS:
            {
                const int divBlobs = 10;
                const int blobs = g.GetQuantity() / divBlobs;
                g.SetRewardByType(MR_BLOBS, blobs);

                const int multMoney = 5;
                const int money = g.GetQuantity() * multMoney;
                g.SetRewardByType(MR_MONEY, money);
            }
            break;

            case MG_DESTROY_ENEMY_BASE:
            {
                const int money = 10000;

                g.SetRewardByType(MR_MONEY, money);
            }
            break;

            case MG_DESTROY_ALL_ENEMIES:
            {
                const int money = 15000;

                g.SetRewardByType(MR_MONEY, money);
            }
            break;

            case MG_GAIN_MONEY:
            {
                const int divBlobs = 100;
                const int blobs = g.GetQuantity() / divBlobs;
                g.SetRewardByType(MR_BLOBS, blobs);

                const int divDiamonds = 100;
                const int diamonds = g.GetQuantity() / divDiamonds;
                g.SetRewardByType(MR_DIAMONDS, diamonds);

                const int divEnergy = 50;
                const int energy = g.GetQuantity() / divEnergy;
                g.SetRewardByType(MR_ENERGY, energy);

                const int divMaterial = 10;
                const int material = g.GetQuantity() / divMaterial;
                g.SetRewardByType(MR_MATERIAL, material);
            }
            break;

            case MG_MINE_MATERIAL:
            {
                const int divEnergy = 10;
                const int energy = g.GetQuantity() / divEnergy;
                g.SetRewardByType(MR_ENERGY, energy);

                const int divMoney = 2;
                const int money = g.GetQuantity() / divMoney;
                g.SetRewardByType(MR_MONEY, money);
            }
            break;

            case MG_MINE_ENERGY:
            {
                const int divMaterial = 10;
                const int material = g.GetQuantity() / divMaterial;
                g.SetRewardByType(MR_MATERIAL, material);

                const int divMoney = 2;
                const int money = g.GetQuantity() / divMoney;
                g.SetRewardByType(MR_MONEY, money);
            }
            break;

            case MG_RESIST_TIME:
            {
                const int blobs = g.GetQuantity();
                g.SetRewardByType(MR_BLOBS, blobs);

                const int diamonds = g.GetQuantity();
                g.SetRewardByType(MR_DIAMONDS, diamonds);

                const int multMoney = 100;
                const int money = g.GetQuantity() * multMoney;
                g.SetRewardByType(MR_MONEY, money);
            }
            break;

            case MG_COMPLETE_TUTORIAL:
            {
                const int blobs = 25;
                g.SetRewardByType(MR_BLOBS, blobs);

                const int diamonds = 25;
                g.SetRewardByType(MR_DIAMONDS, diamonds);

                const int energy = 100;
                g.SetRewardByType(MR_ENERGY, energy);

                const int material = 100;
                g.SetRewardByType(MR_MATERIAL, material);
            }
            break;

            default:
                std::cout << "[WAR] Mission Goal type unknown: " << g.GetType() << std::endl;
            break;
        }
    }
}

void ScreenGame::TrackResourcesForGoals()
{
    mResourceTrackers.resize(NUM_MISSION_REWARDS, 0);

    const Player::Stat resourceIds[NUM_MISSION_REWARDS] =
    {
        Player::BLOBS,
        Player::DIAMONDS,
        Player::ENERGY,
        Player::MATERIAL,
        Player::MONEY
    };

    const MissionReward rewardIds[NUM_MISSION_REWARDS] =
    {
        MR_BLOBS,
        MR_DIAMONDS,
        MR_ENERGY,
        MR_MATERIAL,
        MR_MONEY,
    };

    for(unsigned int i = 0; i < NUM_MISSION_REWARDS; ++i)
    {
        const Player::Stat resId = resourceIds[i];
        const unsigned int rewId = rewardIds[i];

        mResourceTrackers[rewId] = mLocalPlayer->AddOnResourceChanged(resId,
            [this, rewId](const StatValue *, int oldVal, int newVal)
            {
                if(newVal > oldVal)
                    mResourcesGained[rewId] += newVal - oldVal;
            });
    }
}

void ScreenGame::ClearResourcesTracking()
{
    const Player::Stat resourceIds[NUM_MISSION_REWARDS] =
    {
        Player::BLOBS,
        Player::DIAMONDS,
        Player::ENERGY,
        Player::MATERIAL,
        Player::MONEY
    };

    for(unsigned int i = 0; i < NUM_MISSION_REWARDS; ++i)
    {
        const Player::Stat resId = resourceIds[i];
        const int funId = mResourceTrackers[i];

        mLocalPlayer->RemoveOnResourceChanged(resId, funId);
    }
}

void ScreenGame::EndTurn()
{
    Game * game = GetGame();

    // END TURN
    std::cout << "ScreenGame::EndTurn - END PLAYER " << mActivePlayerIdx << "\n" << std::endl;

    // current active player is local player
    if(IsCurrentTurnLocal())
        ClearSelection(mLocalPlayer);

    // START NEW TURN
    const int players = game->GetNumPlayers();

    mActivePlayerIdx = (mActivePlayerIdx + 1) % players;

    std::cout << "ScreenGame::EndTurn - START PLAYER " << mActivePlayerIdx << std::endl;

    // update active player data
    Player * p = game->GetPlayerByIndex(mActivePlayerIdx);
    const PlayerFaction activeFaction = p->GetFaction();

    p->ResetTurnEnergy();
    p->UpdateResources();

    mGameMap->OnNewTurn(activeFaction);

    // new active player is local player
    if(IsCurrentTurnLocal())
    {
        mHUD->ShowTurnControlPanel();

        // reset focus to Stage
        sgl::sgui::Stage::Instance()->SetFocus();
    }
    // new active player is AI
    else
        mHUD->ShowTurnControlTextEnemyTurn();
}

void ScreenGame::PlayLocalActionErrorSFX(const Player * player)
{
    if(player->IsLocal())
    {
        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("game/error_action_01.ogg");
    }
}

} // namespace game
