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
#include "MissionGoalsTracker.h"
#include "Player.h"
#include "AI/ConquerPath.h"
#include "AI/ObjectPath.h"
#include "AI/PlayerAI.h"
#include "AI/WallBuildPath.h"
#include "GameObjects/Base.h"
#include "GameObjects/DefensiveTower.h"
#include "GameObjects/Hospital.h"
#include "GameObjects/MiniUnit.h"
#include "GameObjects/MiniUnitsGroup.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "GameObjects/ResearchCenter.h"
#include "GameObjects/SpawningTower.h"
#include "GameObjects/Temple.h"
#include "GameObjects/Unit.h"
#include "GameObjects/WallGate.h"
#include "GameObjectTools/Weapon.h"
#include "Indicators/AttackRangeIndicator.h"
#include "Indicators/ConquestIndicator.h"
#include "Indicators/HealingRangeIndicator.h"
#include "Indicators/PathIndicator.h"
#include "Indicators/PathOverlay.h"
#include "Indicators/StructureIndicator.h"
#include "Indicators/WallIndicator.h"
#include "Particles/UpdaterDamage.h"
#include "Particles/UpdaterHealing.h"
#include "Particles/UpdaterHitPoints.h"
#include "Particles/UpdaterOutput.h"
#include "Particles/UpdaterSingleLaser.h"
#include "States/StatesIds.h"
#include "Tutorial/TutorialConstants.h"
#include "Tutorial/TutorialManager.h"
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
#include <sgl/utilities/StringManager.h>

#include <cassert>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

namespace game
{

ScreenGame::ScreenGame(Game * game)
    : Screen(game)
    , mPartMan(new sgl::graphic::ParticlesManager)
    , mSM(sgl::utilities::StringManager::Instance())
    , mPathfinder(new sgl::ai::Pathfinder)
    , mCurrCell(-1, -1)
    , mTimerAutoEndTurn(TIME_AUTO_END_TURN)
    , mLocalPlayer(game->GetLocalPlayer())
    , mTurnStage(TURN_STAGE_PLAY)
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


    // MISSION GOALS
    mTrackerMG = new MissionGoalsTracker(game, mLocalPlayer);

    // -- ISOMETRIC MAP --
    CreateIsoMap();
    CreateLayers();

    // create game map
    mGameMap = new GameMap(GetGame(), this, mIsoMap);

    mTrackerMG->SetControlMap(mGameMap->GetControlMap());

    LoadMapFile();

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
    mPathfinder->SetMap(mGameMap);

    // -- PLAYERS --
    const unsigned int numPlayers = game->GetNumPlayers();

    for(int i = 0; i < numPlayers; ++i)
    {
        Player * p = game->GetPlayerByIndex(i);

        p->ResetTurnEnergy();

        // AI players
        if(p->IsAI())
        {
            p->GetAI()->SetGameMap(mGameMap);
            mAiPlayers.push_back(p);

            p->ResetTurnsPlayed();
        }
        // local player
        else
        {
            // first turn is always played by local player
            p->ResetTurnsPlayed(1);
        }
    }

    // UI
    CreateUI();

    mTrackerMG->SetGameHUD(mHUD);

    // OVERLAYS
    mPathOverlay = new PathOverlay(mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS4),
                                   mIsoMap->GetNumRows(), mIsoMap->GetNumCols());

    mPathIndicator = new PathIndicator(mLocalPlayer->GetFaction(), true);

    // set initial camera position
    CenterCameraOverObject(mLocalPlayer->GetBase());

    // apply initial visibility to the game map
    mGameMap->InitVisibility(mLocalPlayer);

    InitMusic();

    // TUTORIAL
    if(game->IsTutorialEnabled())
    {
        auto tutMan = game->GetTutorialManager();

        if(tutMan->GetTutorialState(TUTORIAL_MISSION_INTRO) == TS_TODO)
        {
            tutMan->CreateTutorial(TUTORIAL_MISSION_INTRO, this);
            tutMan->StartTutorial();

            mTrackerMG->SetTutorialStarted();
        }
    }
}

ScreenGame::~ScreenGame()
{
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

    delete mPathOverlay;

    delete mPathIndicator;

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

    delete mTrackerMG;

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
    // always move CAMERA
    mCamController->Update(delta);

    // always continue the TUTORIAL
    GetGame()->GetTutorialManager()->Update(delta);

    // do nothing else when paused
    if(mPaused)
        return ;

    // keep track of time played (while not paused)
    mTimePlayed += delta;
    mTrackerMG->SetPlayedTime(mTimePlayed);

    // -- PARTICLES --
    mPartMan->Update(delta);

    // -- AUTO END TURN --
    const float minEn = 1.f;
    const Game * game = GetGame();

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
    if(!IsCurrentTurnLocal())
         UpdateAI(delta);

    // check game end
    UpdateGameEnd();
}

void ScreenGame::Render()
{
    mIsoMap->Render();

    mPartMan->Render();
}

void ScreenGame::OnObjectDestroyed(GameObject * obj)
{
    // clear action in progress
    ClearObjectAction(obj);

    // clear selection if object is selected
    if(obj->IsSelected())
    {
        Player * owner = GetGame()->GetPlayerByFaction(obj->GetFaction());
        ClearSelection(owner);
    }

    // remove object from mini map
    mHUD->GetMinimap()->RemoveElement(obj->GetRow0(), obj->GetCol0());
}

void ScreenGame::ClearSelection(Player * player)
{
    GameObject * selObj = player->GetSelectedObject();

    // no selection -> nothing to do
    if(nullptr == selObj)
        return ;

    mHUD->ClearQuickUnitButtonChecked();
    mHUD->HidePanelHit();
    mHUD->HidePanelObjectActions();
    mHUD->HidePanelSelectedObject();
    mHUD->HideDialogNewElement();

    player->ClearSelectedObject();

    ClearCellOverlays();
    HideActionPanels();
}

void ScreenGame::SelectObject(GameObject * obj, Player * player)
{
    auto ap = sgl::media::AudioManager::Instance()->GetPlayer();

    player->SetSelectedObject(obj);

    const auto cat = obj->GetObjectCategory();

    // update quick selection buttons when selected unit
    if(cat == ObjectData::CAT_UNIT)
    {
        mHUD->SetQuickUnitButtonChecked(obj);

        // show current indicator
        ShowActiveUnitIndicators(static_cast<Unit *>(obj), mCurrCell);

        ap->PlaySound("game/selection-01.ogg");
    }
    // not a unit
    else
    {
        if(cat == ObjectData::CAT_STRUCTURE)
            ap->PlaySound("game/selection-02.ogg");
        else
            ap->PlaySound("game/selection-03.ogg");

        mHUD->ClearQuickUnitButtonChecked();

        // show attack range overlay for towers
        const GameObjectTypeId type = obj->GetObjectType();

        if(type == ObjectData::TYPE_DEFENSIVE_TOWER || type == ObjectData::TYPE_BUNKER)
        {
            const int range = obj->GetWeapon()->GetRange();
            ShowAttackIndicators(obj, range);
        }
        else if(obj->GetObjectCategory() == ObjectData::CAT_MINI_UNIT)
        {
            auto group = static_cast<MiniUnitsGroup *>(obj->GetGroup());

            if(group->HasPathSet())
                mPathOverlay->SetPath(group->GetPath(), obj->GetFaction());
        }
    }

    mHUD->ShowPanelObjectActions(obj);
    mHUD->ShowPanelSelectedObject(obj);

    // set UI focus
    sgl::sgui::Stage::Instance()->SetFocus();
}

void ScreenGame::CenterCameraOverObject(const GameObject * obj)
{
    if(nullptr == obj)
        return ;

    const IsoObject * isoObj = obj->GetIsoObject();
    const int cX = isoObj->GetX() + isoObj->GetWidth() / 2;
    const int cY = isoObj->GetY() + isoObj->GetHeight() / 2;

    mCamController->CenterCameraToPoint(cX, cY);

    // update current cell like if mouse was moved
    UpdateCurrentCell();
}

Player * ScreenGame::GetActivePlayer() const
{
    return GetGame()->GetPlayerByIndex(mActivePlayerIdx);
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

    // pause sounds that are playing
    auto ap = sgl::media::AudioManager::Instance()->GetPlayer();

    if(mPaused)
        ap->PauseSounds();
    else
        ap->ResumeSounds();

    mHUD->SetEnabled(!paused);

    mHUD->UpdatePanelTurnControl();
}

void ScreenGame::SetLocalTurnStage(TurnStage ts)
{
    // only set stage for local player
    if(!IsCurrentTurnLocal())
        return ;

    mTurnStage = ts;

    mHUD->UpdatePanelTurnControl();
}

bool ScreenGame::CanLocalPlayerInteract() const
{
    return IsCurrentTurnLocal() && !mGameMap->IsDoingAutomaticMoves();
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

    // HIT POINTS
    updater = new UpdaterHitPoints;
    mPartMan->RegisterUpdater(PU_HIT_POINTS, updater);

    // LOOTBOX PRIZE
    updater = new UpdaterOutput;
    mPartMan->RegisterUpdater(PU_OUTPUT, updater);

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
    mIsoMap->CreateLayer(MapLayers::GROUND_OBJECTS);
    mIsoMap->CreateLayer(MapLayers::REGULAR_OBJECTS);
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
        HideActionPanels();
    });

    // build wall
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_BUILD_WALL, [this]
    {
        auto unit = static_cast<Unit *>(mLocalPlayer->GetSelectedObject());
        unit->SetActiveAction(GameObjectActionType::BUILD_WALL);

        ClearCellOverlays();
        HideActionPanels();

        mWallPath.clear();

        ShowBuildWallIndicator(unit, mCurrCell);
    });

    // attack
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_ATTACK, [this]
    {
        auto unit = static_cast<Unit *>(mLocalPlayer->GetSelectedObject());
        unit->SetActiveAction(GameObjectActionType::ATTACK);

        ClearCellOverlays();
        HideActionPanels();

        mHUD->ShowPanelShotType();
        UpdatePanelHit(unit);

        // show attack range overlay
        const int range = unit->GetWeapon()->GetRange();
        ShowAttackIndicators(unit, range);
    });

    // heal
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_HEAL_HOSPITAL, [this]
    {
        auto hospital = static_cast<Hospital *>(mLocalPlayer->GetSelectedObject());
        hospital->SetActiveAction(GameObjectActionType::HEAL);

        ClearCellOverlays();
        HideActionPanels();

        // show healing range overlay
        const int range = hospital->GetRangeHealing();
        ShowHealingIndicators(hospital, range);
    });

    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_HEAL_UNIT, [this]
    {
        auto unit = static_cast<Unit *>(mLocalPlayer->GetSelectedObject());
        unit->SetActiveAction(GameObjectActionType::HEAL);

        ClearCellOverlays();
        HideActionPanels();

        // show healing range overlay
        const int range = unit->GetHealingRange();
        ShowHealingIndicators(unit, range);
    });

    // conquer
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_CONQUER_CELL, [this]
    {
        auto unit = static_cast<Unit *>(mLocalPlayer->GetSelectedObject());
        unit->SetActiveAction(GameObjectActionType::CONQUER_CELL);

        ClearCellOverlays();
        HideActionPanels();

        mConquestPath.clear();

        ShowConquestIndicator(unit, mCurrCell);
    });

    // move
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_MOVE, [this]
    {
        auto unit = static_cast<Unit *>(mLocalPlayer->GetSelectedObject());
        unit->SetActiveAction(GameObjectActionType::MOVE);

        ClearCellOverlays();
        HideActionPanels();

        ShowMoveIndicator(unit, mCurrCell);
    });

    // spawn mini-units
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_SPAWN, [this]
    {
        auto selObj = mLocalPlayer->GetSelectedObject();
        selObj->SetActiveAction(GameObjectActionType::SPAWN);

        ClearCellOverlays();
        HideActionPanels();

        mHUD->ShowDialogNewMiniUnitsSquad(selObj);
    });

    // set target destination for mini units
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_SET_TARGET, [this]
    {
        auto mu = mLocalPlayer->GetSelectedObject();

        auto group = static_cast<MiniUnitsGroup *>(mu->GetGroup());

        group->DoForAll([](GameObject * obj)
        {
            obj->SetActiveAction(GameObjectActionType::SET_TARGET);
        });

        ShowActiveMiniUnitIndicators(static_cast<MiniUnit *>(mu), mCurrCell);
    });

    // control research of research center
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_RESEARCH, [this]
    {
        auto rc = static_cast<ResearchCenter *>(mLocalPlayer->GetSelectedObject());
        mHUD->ShowDialogResearch(rc);
    });

    // open tech tree
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_TECH_TREE, [this]
    {
        mHUD->ShowDialogTechTree();
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
        mIsoMap->ChangeObjectLayer(gate->GetIsoObject(), MapLayers::REGULAR_OBJECTS,
                                   MapLayers::GROUND_OBJECTS);

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
        mIsoMap->ChangeObjectLayer(gate->GetIsoObject(), MapLayers::GROUND_OBJECTS, MapLayers::REGULAR_OBJECTS);

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
    // self destruction
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_SELF_DESTROY, [this]
    {
        GameObject * selObj = mLocalPlayer->GetSelectedObject();
        selObj->SetActiveAction(SELF_DESTRUCTION);

        ClearCellOverlays();
        HideActionPanels();

        mHUD->ShowPanelSelfDestruction();
    });

    // upgrade
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_UPGRADE, [this]
    {
        GameObject * selObj = mLocalPlayer->GetSelectedObject();
        mHUD->ShowDialogUpgrade(selObj);
    });

    // cancel
    panelObjActions->AddButtonFunction(PanelObjectActions::BTN_CANCEL,
                                       [this, panelObjActions]
    {
        GameObject * selObj = mLocalPlayer->GetSelectedObject();

        if(nullptr == selObj)
            return ;

        // always hide panel self destruction
        mHUD->HidePanelSelfDestruction();

        const GameObjectActionType action = selObj->GetActiveAction();

        // special case MiniUnits
        if(selObj->GetObjectCategory() == ObjectData::CAT_MINI_UNIT)
            CancelMiniUnitsGroupPath(selObj->GetGroup());

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
            ShowActiveUnitIndicators(static_cast<Unit *>(selObj), mCurrCell);

            return ;
        }
        else if(action == SELF_DESTRUCTION)
            selObj->SetActiveActionToDefault();

        CancelObjectAction(selObj);
    });

    // MISSION COUNTDOWN if needed
    const std::vector<MissionGoal> goals = mTrackerMG->GetGoals();

    for(const MissionGoal & g : goals)
    {
        if(g.GetType() == MissionGoal::TYPE_RESIST_TURNS)
        {
            mHUD->ShowMissionCountdown(g.GetQuantity());
            break;
        }
    }

    // set initial focus to Stage
    sgl::sgui::Stage::Instance()->SetFocus();
}

void ScreenGame::HideActionPanels()
{
    mHUD->HidePanelSelfDestruction();
    mHUD->HidePanelShotType();

    mHUD->HidePanelHit();
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

    mGameMap->InitCities();

    // get mission data
    mTrackerMG->SetGoals(mio.GetMissionGoals());
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

    if(key == KeyboardEvent::KEY_ESCAPE)
        mHUD->ShowDialogExit();
    // SHIFT + B -> center camera on own base
    else if(key == KeyboardEvent::KEY_B)
    {
        if(event.IsModShiftDown())
            CenterCameraOverObject(mLocalPlayer->GetBase());
    }
#ifdef DEV_MODE
    // DEBUG: SHIFT+N -> add Experience to selected object
    else if(event.IsModShiftDown() && key == KeyboardEvent::KEY_N)
    {
        auto selObj = GetGame()->GetLocalPlayer()->GetSelectedObject();

        if(selObj != nullptr)
            selObj->SumExperience(1000);
    }
    // DEBUG: ALT + U -> toggle UI
    else if(event.IsModAltDown() && key == KeyboardEvent::KEY_U)
        mHUD->SetVisible(!mHUD->IsVisible());
    // DEBUG: CTRL+P -> Upgrade screen
    else if(event.IsModCtrlDown() && key == KeyboardEvent::KEY_P)
    {
        auto selObj = GetGame()->GetLocalPlayer()->GetSelectedObject();

        if(selObj != nullptr)
            mHUD->ShowDialogUpgrade(selObj);
    }
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
            if(o->GetObjectType() == ObjectData::TYPE_TEMPLE)
            {
                mHUD->ShowDialogExploreTemple(mLocalPlayer, static_cast<Temple *>(o));

                break;
            }
        }
    }
    // DEBUG: show dialog trading
    else if(event.IsModShiftDown() && key == KeyboardEvent::KEY_T)
        mHUD->ShowDialogTrading();
    // DEBUG: add enemy on current cell
    else if(event.IsModShiftDown() && key == KeyboardEvent::KEY_E)
        CreateEnemyInCurrentCell(ObjectData::TYPE_UNIT_SOLDIER1);
    else if(event.IsModAltDown() && key == KeyboardEvent::KEY_E)
        CreateEnemyInCurrentCell(ObjectData::TYPE_UNIT_SOLDIER2);
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
    if(!CanLocalPlayerInteract())
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
    if(!CanLocalPlayerInteract())
        return ;

    UpdateCurrentCell();
}

void ScreenGame::OnWindowExposed(sgl::graphic::WindowEvent &)
{
}

void ScreenGame::OnWindowHidden(sgl::graphic::WindowEvent &)
{
    mCamController->HandleMouseLeftWindow();

    if(!mHUD->IsShowingDialog())
        mHUD->ShowDialogExit();
}

void ScreenGame::OnWindowMouseEntered(sgl::graphic::WindowEvent & event)
{
}

void ScreenGame::OnWindowMouseLeft(sgl::graphic::WindowEvent & event)
{
    mCamController->HandleMouseLeftWindow();

    if(!mHUD->IsShowingDialog())
        mHUD->ShowDialogExit();
}

void ScreenGame::OnAutomaticMovesFinished()
{
    if(IsCurrentTurnLocal())
        InitLocalTurn();
}

void ScreenGame::UpdateAI(float delta)
{
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

    auto PrintAction = [this](int turnAI, const ActionAI * action, bool done, Player * player)
    {
        std::cout << "ScreenGame::ExecuteAIAction - AI " << turnAI << " - "
                  << action->GetTypeStr()
                  << (done ? " DOING" : " FAILED")
                  << " | ACT ID: " << action->actId
                  << " - PRIORITY: " << action->priority;

        // make sure obj is still valid (not destroyed)
        if(action->ObjSrc != nullptr && mGameMap->HasObject(action->ObjSrc))
            std::cout << " | OBJ ID: " << action->ObjSrc->GetObjectId()
                      << " - OBJ ENERGY: " << action->ObjSrc->GetEnergy();

        std::cout << " | TURN ENERGY: " << player->GetTurnEnergy() << std::endl;
    };

    // execute planned action until one is successful or there's no more actions to do (NOP)
    while(!done)
    {
        const ActionAI * action = ai->GetNextActionTodo();

        if(nullptr == action || AIA_END_TURN == action->type)
        {
            std::cout << "ScreenGame::ExecuteAIAction - AI " << turnAI
                      << " ==================== END TURN ====================" << std::endl;

            if(action != nullptr)
                delete action;

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
            case AIA_UPGRADE_UNIT:
            {
                auto a = static_cast<const ActionAIUpgradeObject *>(action);
                auto unit = static_cast<Unit *>(a->ObjSrc);

                unit->UpgradeLevel(a->attChanges);

                // keep doing things
                done = false;

                PrintAction(turnAI, action, true, player);
            }
            break;

            case AIA_UNIT_ATTACK_ENEMY_UNIT:
            {
                auto unit = static_cast<Unit *>(action->ObjSrc);
                done = SetupUnitAttack(unit, action->ObjDst, player, basicOnDone);

                PrintAction(turnAI, action, done, player);
            }
            break;

            case AIA_UNIT_CONQUER_STRUCT:
            {
                auto unit = static_cast<Unit *>(action->ObjSrc);
                const Cell2D start(unit->GetRow0(), unit->GetCol0());
                const Cell2D end(action->ObjDst->GetRow0(), action->ObjDst->GetCol0());

                // NOTE probably need to change actions to not take into consideration cells,
                // but only objects

                // unit and generator are next to each other
                if(mGameMap->AreObjectsOrthoAdjacent(action->ObjSrc, action->ObjDst))
                    done = SetupStructureConquest(unit, start, end, player, basicOnDone);
                // unit needs to move to the generator
                else
                {
                    Cell2D target = mGameMap->GetOrthoAdjacentMoveTarget(start, action->ObjDst);

                    // failed to find a suitable target
                    if(-1 == target.row || -1 == target.col)
                        done = false;
                    else
                    {
                        done = SetupUnitMove(unit, start, target, true,
                            [this, unit, end, player, basicOnDone](bool successful)
                            {
                                if(successful)
                                {
                                    const Cell2D currCell(unit->GetRow0(), unit->GetCol0());
                                    const bool res = SetupStructureConquest(unit, currCell, end,
                                                                            player, basicOnDone);

                                    if(!res)
                                        basicOnDone(false);
                                }
                                else
                                    basicOnDone(false);
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
                    done = SetupConnectCellsAI(unit, basicOnDone);
                // unit needs to move to the structure
                else
                {
                    done = SetupUnitMove(unit, unitCell, start, true,
                        [this, unit, start, basicOnDone](bool successful)
                        {
                            if(successful)
                            {
                                const bool res = SetupConnectCellsAI(unit, basicOnDone);

                                if(!res)
                                    basicOnDone(false);
                            }
                            else
                                basicOnDone(false);
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

            case AIA_UNIT_PATROL:
            {
                auto unit = static_cast<Unit *>(action->ObjSrc);

                const Cell2D cellUnit(unit->GetRow0(), unit->GetCol0());

                done = SetupUnitMove(unit, cellUnit, action->cellDst, true, basicOnDone);

                PrintAction(turnAI, action, done, player);
            }
            break;

            case AIA_UNIT_COLLECT_BLOBS:
            {
                auto unit = static_cast<Unit *>(action->ObjSrc);

                const Cell2D cellUnit(unit->GetRow0(), unit->GetCol0());
                const Cell2D cellDest(action->ObjDst->GetRow0(), action->ObjDst->GetCol0());

                done = SetupUnitMove(unit, cellUnit, cellDest, true, basicOnDone);

                PrintAction(turnAI, action, done, player);
            }
            break;

            case AIA_UNIT_COLLECT_DIAMONDS:
            {
                auto unit = static_cast<Unit *>(action->ObjSrc);

                const Cell2D cellUnit(unit->GetRow0(), unit->GetCol0());
                const Cell2D cellDest(action->ObjDst->GetRow0(), action->ObjDst->GetCol0());

                done = SetupUnitMove(unit, cellUnit, cellDest, true, basicOnDone);

                PrintAction(turnAI, action, done, player);
            }
            break;

            case AIA_UNIT_COLLECT_LOOTBOX:
            {
                auto unit = static_cast<Unit *>(action->ObjSrc);

                const Cell2D cellUnit(unit->GetRow0(), unit->GetCol0());
                const Cell2D cellDest(action->ObjDst->GetRow0(), action->ObjDst->GetCol0());

                done = SetupUnitMove(unit, cellUnit, cellDest, true, basicOnDone);

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

                if(ai->FindWhereToBuildStructure(unit, target))
                {
                    if(mGameMap->AreCellsAdjacent(cellUnit, target))
                        done = SetupStructureBuilding(unit, target, player, basicOnDone);
                    else
                    {
                        const Cell2D moveTarget = mGameMap->GetAdjacentMoveTarget(cellUnit, target);

                        if(moveTarget.row != -1 && moveTarget.col != -1)
                        {
                            done = SetupUnitMove(unit, cellUnit, moveTarget, true,
                                [this, unit, target, player, basicOnDone](bool successful)
                                {
                                    if(successful)
                                    {
                                        const bool res = SetupStructureBuilding(unit, target,
                                                                                player, basicOnDone);

                                        if(!res)
                                        {
                                            basicOnDone(false);
                                            unit->ClearStructureToBuild();
                                        }
                                    }
                                    else
                                    {
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

            case AIA_UNIT_BUILD_TOWER:
            {
                auto a = static_cast<const ActionAIBuildStructure *>(action);

                // decide where to build
                auto unit = static_cast<Unit *>(a->ObjSrc);
                unit->SetStructureToBuild(a->structType);

                const Cell2D cellUnit(unit->GetRow0(), unit->GetCol0());
                Cell2D target;

                if(ai->FindWhereToBuildTower(unit, target))
                {
                    if(mGameMap->AreCellsAdjacent(cellUnit, target))
                        done = SetupStructureBuilding(unit, target, player, basicOnDone);
                    else
                    {
                        const Cell2D moveTarget = mGameMap->GetAdjacentMoveTarget(cellUnit, target);

                        if(moveTarget.row != -1 && moveTarget.col != -1)
                        {
                            done = SetupUnitMove(unit, cellUnit, moveTarget, true,
                                [this, unit, target, player, basicOnDone](bool successful)
                                {
                                    if(successful)
                                    {
                                        const bool res = SetupStructureBuilding(unit, target,
                                                                                player, basicOnDone);

                                        if(!res)
                                        {
                                            basicOnDone(false);
                                            unit->ClearStructureToBuild();
                                        }
                                    }
                                    else
                                    {
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
                        done = SetupUnitMove(unit, start, dest, true,
                            [this, unit, target, player, basicOnDone](bool successful)
                            {
                                if(successful)
                                {
                                    const bool res = SetupUnitAttack(unit, target, player, basicOnDone);

                                    if(!res)
                                        basicOnDone(false);
                                }
                                else
                                    basicOnDone(false);
                            });
                    }
                    else
                        done = false;
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
        {
            if(action->ObjSrc != nullptr && action->ObjSrc->IsVisible())
                CenterCameraOverObject(action->ObjSrc);

            ai->RegisterActionInProgress(action);
        }
        else
            delete action;
    }
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

void ScreenGame::CancelObjectAction(GameObject * obj)
{
    auto ap = sgl::media::AudioManager::Instance()->GetPlayer();

    auto it = mObjActions.begin();

    // search selected object in active actions
    while(it != mObjActions.end())
    {
        GameObjectAction & act = *it;

        if(act.obj == obj)
        {
            const GameObjectTypeId objType = act.obj->GetObjectType();
            const GameObjectActionType actType = act.type;

            // spawn mini-units
            if(actType == GameObjectActionType::SPAWN)
            {
                act.progressBar->DeleteLater();
                ap->StopSound("game/build-04.ogg");
            }
            // building a new unit
            else if(actType == GameObjectActionType::BUILD_UNIT)
            {
                act.progressBar->DeleteLater();
                ap->StopSound("game/build-03.ogg");
            }

            // object is a Unit
            if(act.obj->GetObjectCategory() == ObjectData::CAT_UNIT)
            {
                if(actType == GameObjectActionType::MOVE)
                    mGameMap->AbortMove(obj);
                else if(actType == GameObjectActionType::CONQUER_CELL)
                    mGameMap->AbortCellConquest(obj);
                else if(actType == GameObjectActionType::BUILD_WALL)
                    mGameMap->AbortBuildWalls(obj);
                else if(actType == GameObjectActionType::CONQUER_STRUCTURE)
                {
                    act.progressBar->DeleteLater();
                    ap->StopSound("game/conquer-02.ogg");
                }
                else if(actType == GameObjectActionType::ATTACK)
                {
                    auto unit = static_cast<Unit *>(obj);
                    unit->ClearTargetAttack();
                }
                // building a structure
                else if(actType == GameObjectActionType::BUILD_STRUCTURE)
                {
                    act.progressBar->DeleteLater();
                    ap->StopSound("game/build-01.ogg");
                }
                else if(actType == GameObjectActionType::HEAL)
                {
                    auto unit = static_cast<Unit *>(obj);
                    unit->ClearTargetHealing();
                }
            }
            // object is a Hospital
            else if(objType == ObjectData::TYPE_HOSPITAL)
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
                if(obj->GetObjectCategory() == ObjectData::CAT_UNIT)
                    ShowActiveUnitIndicators(static_cast<Unit *>(act.obj), mCurrCell);
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

void ScreenGame::CancelMiniUnitsGroupPath(GameObjectsGroup * group)
{
    auto muGroup = dynamic_cast<MiniUnitsGroup *>(group);

    if(nullptr == muGroup)
        return ;

    mPathOverlay->ClearPath();

    muGroup->ClearPath();

    muGroup->DoForAll([](GameObject * obj)
    {
        obj->SetActiveAction(GameObjectActionType::IDLE);
        obj->SetCurrentAction(GameObjectActionType::IDLE);
    });
}

void ScreenGame::UpdateGameEnd()
{
    // check if player has base for instant GAME OVER
    if(CheckGameOverForLocalPlayer())
    {
        mHUD->ShowDialogEndMission(false);
        return ;
    }

    mTrackerMG->Update();
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
    const PlanetId planet = game->GetCurrentPlanet();

    mapReg->SetMapOccupier(planet, territory, faction);
    mapReg->SetMapStatus(planet, territory, TER_ST_OCCUPIED);

    if(faction == mLocalPlayer->GetFaction())
        mapReg->SetMapMissionCompleted(planet, territory);

    game->RequestNextActiveState(StateId::PLANET_MAP);
}

bool ScreenGame::CheckGameOverForLocalPlayer()
{
    // check if player still has base
    return !mLocalPlayer->HasStructure(ObjectData::TYPE_BASE);
}

int ScreenGame::CellToIndex(const Cell2D & cell) const
{
    return cell.row * mIsoMap->GetNumCols() + cell.col;
}

bool ScreenGame::SetupNewMiniUnits(GameObjectTypeId type, GameObject * gen, GameObjectsGroup * group,
                                   Player * player, int squads, int elements,
                                   const std::function<void(bool)> & onDone)
{
    // check if create is possible
    if(!mGameMap->CanCreateMiniUnit(type, gen, elements, player))
        return false;

    // find where to build
    const Cell2D gc(gen->GetRow0(), gen->GetCol0());
    const Cell2D cell = mGameMap->GetNewMiniUnitDestination(gc);

    if(-1 == cell.row || -1 == cell.col)
    {
        gen->ShowWarning(mSM->GetCString("WARN_CANT_CELL"), 3.f);
        return false;
    }

    // set time to build
    float timeSpawn = 0.f;

    if(gen->GetObjectCategory() == ObjectData::CAT_UNIT)
        timeSpawn = static_cast<Unit *>(gen)->GetTimeSpawnMiniUnit();
    else if(gen->GetObjectType() == ObjectData::TYPE_SPAWN_TOWER)
        timeSpawn = static_cast<SpawningTower *>(gen)->GetTimeSpawnMiniUnit();

    // no group set yet -> create one
    if(nullptr == group)
        group = mGameMap->CreateMiniUnitsGroup(gen->GetFaction());

    GameMapProgressBar * pb = mHUD->CreateProgressBarInCell(cell, timeSpawn, player->GetFaction());

    pb->AddFunctionOnCompleted([this, cell, player, gen, type, elements, squads, group]
    {
        gen->ActionStepCompleted(SPAWN);
        gen->SetCurrentAction(GameObjectActionType::IDLE);

        auto mu = mGameMap->CreateMiniUnit(type, gen, cell, elements, player);

        assert(mu != nullptr);

        if(player->IsLocal())
            mTrackerMG->AddMiniUnitCreated();

        mu->SetGroup(group);

        // add unit to map if cell is visible to local player
        if(mGameMap->IsCellVisibleToLocalPlayer(cell.row, cell.col))
            AddObjectToMinimap(cell, type, player->GetFaction());

        SetObjectActionCompleted(gen);

        // stop sound
        auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
        ap->FadeOutSound("game/build-04.ogg", 125);

        if(squads > 1)
            SetupNewMiniUnits(type, gen, group, mLocalPlayer, squads - 1, elements);
    });

    // store active action
    mObjActionsToDo.emplace_back(gen, GameObjectActionType::SPAWN, cell, pb, onDone);

    gen->SetActiveAction(GameObjectActionType::IDLE);
    gen->SetCurrentAction(GameObjectActionType::SPAWN);

    // disable actions panel (if action is done by local player)
    if(player->IsLocal())
        mHUD->SetLocalActionsEnabled(false);

    // play sound
    if(gen->IsVisible())
    {
        auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
        ap->PlaySoundLoop("game/build-04.ogg");
    }

    return true;
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
        gen->ShowWarning(mSM->GetCString("WARN_CANT_CELL"), 3.f);
        return false;
    }

    // start create
    mGameMap->StartCreateUnit(type, gen, cell, player);

    // create and init progress bar
    assert(gen->IsStructure());

    auto pb = mHUD->CreateProgressBarInCell(cell, static_cast<Structure *>(gen)->GetTimeBuildUnit(),
                                            player->GetFaction());

    pb->AddFunctionOnCompleted([this, cell, player, gen, type]
    {
        gen->ActionStepCompleted(BUILD_UNIT);
        gen->SetCurrentAction(GameObjectActionType::IDLE);

        mGameMap->CreateUnit(type, cell, player);

        if(player->IsLocal())
            mTrackerMG->AddUnitCreated();

        // add unit to map if cell is visible to local player
        if(mGameMap->IsCellVisibleToLocalPlayer(cell.row, cell.col))
            AddObjectToMinimap(cell, type, player->GetFaction());

        SetObjectActionCompleted(gen);

        // stop sound
        auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
        ap->FadeOutSound("game/build-03.ogg", 250);
    });

    // store active action
    mObjActionsToDo.emplace_back(gen, GameObjectActionType::BUILD_UNIT, cell, pb, onDone);

    gen->SetActiveAction(GameObjectActionType::IDLE);
    gen->SetCurrentAction(GameObjectActionType::BUILD_UNIT);

    // disable actions panel (if action is done by local player)
    if(player->IsLocal())
        mHUD->SetLocalActionsEnabled(false);

    // play sound
    if(gen->IsVisible())
    {
        auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
        ap->PlaySoundLoop("game/build-03.ogg");
    }

    return true;
}

bool ScreenGame::SetupStructureConquest(Unit * unit, const Cell2D & start, const Cell2D & end,
                                        Player * player, const std::function<void(bool)> & onDone)
{
    // check if conquest is possible
    if(!mGameMap->CanConquerStructure(unit, end, player))
        return false;

    const GameMapCell & gameCell = mGameMap->GetCell(end.row, end.col);
    GameObject * target = gameCell.objTop;

    // handle special case: TEMPLE
    if(player->IsLocal())
    {
        if(target->GetObjectType() == ObjectData::TYPE_TEMPLE)
        {
            mHUD->ShowDialogExploreTemple(player, static_cast<Temple *>(target));
            return false;
        }
    }

    // start conquest
    mGameMap->StartConquerStructure(end, player);

    // create and init progress bar
    auto pb = mHUD->CreateProgressBarInCell(start, unit->GetTimeConquestStructure(), player->GetFaction());

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

        // register conquest
        mTrackerMG->AddStructureConquered(objStruct->GetObjectType());

        auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
        ap->FadeOutSound("game/conquer-02.ogg", 250);
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

    // play sound
    if(unit->IsVisible())
    {
        auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
        ap->PlaySoundLoop("game/conquer-02.ogg");
    }

    return true;
}

bool ScreenGame::SetupStructureBuilding(Unit * unit, const Cell2D & cellTarget, Player * player,
                                        const std::function<void(bool)> & onDone)
{
    const GameObjectTypeId st = unit->GetStructureToBuild();

    // check if building is possible
    if(!mGameMap->CanBuildStructure(unit, cellTarget, player, st))
        return false;

    mGameMap->StartBuildStructure(cellTarget, player, st);

    // create and init progress bar
    auto pb = mHUD->CreateProgressBarInCell(cellTarget, unit->GetTimeBuildStructure(), player->GetFaction());

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

        if(player->IsLocal())
            mTrackerMG->AddStructureBuilt(st);

        auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
        ap->FadeOutSound("game/build-01.ogg", 250);
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

    // play sound
    if(unit->IsVisible())
    {
        auto ap = sgl::media::AudioManager::Instance()->GetPlayer();
        ap->PlaySoundLoop("game/build-01.ogg");
    }

    return true;
}

bool ScreenGame::SetupUnitAttack(Unit * unit, GameObject * target, Player * player,
                                 const std::function<void(bool)> & onDone)
{
    const bool res = unit->SetTargetAttack(target);

    if(!res)
    {
        unit->ShowWarning(mSM->GetCString("WARN_CANT_TARGET"), 3.f);
        return false;
    }

    unit->SetActiveAction(GameObjectActionType::IDLE);
    unit->SetCurrentAction(GameObjectActionType::ATTACK);

    // disable actions panel and clear overlays (if action is done by local player)
    if(player->IsLocal())
    {
        mHUD->SetLocalActionsEnabled(false);
        mHUD->HidePanelHit();

        ClearCellOverlays();
        HideActionPanels();
    }

    mObjActionsToDo.emplace_back(unit, GameObjectActionType::ATTACK, onDone);

    return true;
}

bool ScreenGame::SetupHospitalHeal(Hospital * hospital, GameObject * target, Player * player,
                                   const std::function<void(bool)> & onDone)
{
    const bool res = hospital->SetTargetHealing(target);

    if(!res)
    {
        hospital->ShowWarning(mSM->GetCString("WARN_CANT_TARGET"), 3.f);
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
        unit->ShowWarning(mSM->GetCString("WARN_CANT_TARGET"), 3.f);
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
                               bool ignoreEnergy, const std::function<void(bool)> & onDone)
{
    const auto path = mPathfinder->MakePath(start.row, start.col, end.row, end.col,
                                            sgl::ai::Pathfinder::ALL_OPTIONS);

    const Player * player = GetGame()->GetPlayerByFaction(unit->GetFaction());

    // empty path -> exit
    if(path.empty())
    {
        unit->ShowWarning(mSM->GetCString("WARN_CANT_PATH2C"), 3.f);
        return false;
    }

    auto op = new ObjectPath(unit, mIsoMap, mGameMap, this);
    op->SetPath(path);

    // do not move if energy is not enough
    const int cost = op->GetPathCost();

    // check energy left before starting the move
    if(!ignoreEnergy)
    {
        if(cost > unit->GetEnergy())
        {
            delete op;
            unit->ShowWarning(mSM->GetCString("WARN_NO_ENE"), 2.f);
            return false;
        }

        if(cost > player->GetTurnEnergy())
        {
            delete op;
            unit->ShowWarning(mSM->GetCString("WARN_NO_T_ENE"), 3.f);
            return false;
        }
    }

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
        delete op;
        unit->ShowWarning(mSM->GetCString("WARN_MOV_FAIL"), 2.f);
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

                delete cp;

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

            delete cp;

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

        delete cp;

        return false;
    }
}

void ScreenGame::HandleUnitMoveOnMouseUp(Unit * unit, const Cell2D & clickCell)
{
    // check destination is visible
    const int ClickInd = clickCell.row * mGameMap->GetNumCols() + clickCell.col;
    const bool clickVisible = mLocalPlayer->IsCellVisible(ClickInd);

    if(!clickVisible)
    {
        unit->ShowWarning(mSM->GetCString("WARN_CELL_NVIS"), 2.f);
        return;
    }

    const Cell2D selCell(unit->GetRow0(), unit->GetCol0());

    // check destination is walkable
    const bool clickWalkable = mGameMap->IsCellWalkable(clickCell.row, clickCell.col);

    // destination is walkable -> try to generate a path and move
    if(clickWalkable)
    {
        SetupUnitMove(unit, selCell, clickCell, false);
        return ;
    }

    // handle special cases for non-walkable cells
    const GameMapCell & clickGameCell = mGameMap->GetCell(ClickInd);
    const GameObject * clickObj = clickGameCell.objTop;

    // there's an object and it can't be conquered -> exit
    if(!clickObj->CanBeConquered())
    {
        unit->ShowWarning(mSM->GetCString("WARN_CANT_BE_CONQUERED"), 3.f);
        return ;
    }

    // unit can't conquer
    if(!unit->CanConquer())
    {
        unit->ShowWarning(mSM->GetCString("WARN_CANT_CONQUER"), 2.f);
        return ;
    }

    // object is adjacent -> try to interact
    if(mGameMap->AreObjectsAdjacent(unit, clickObj))
        SetupStructureConquest(unit, selCell, clickCell, mLocalPlayer);
    // object is far -> move close and then try to conquer
    else
    {
        Cell2D target = mGameMap->GetAdjacentMoveTarget(selCell, clickObj);

        // failed to find a suitable target
        if(-1 == target.row || -1 == target.col)
        {
            unit->ShowWarning(mSM->GetCString("WARN_CANT_CELL"), 3.f);
            return ;
        }

        SetupUnitMove(unit, selCell, target, false,
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

            IsoLayer * layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS4);
            layer->AddObject(mTempStructIndicator, clickCell.row, clickCell.col);

            // move
            SetupUnitMove(unit, cellUnit, target, false,
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
        unit->ShowWarning(mSM->GetCString("WARN_CELL_NOT_VALID"), 3.f);
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
                    unit->ShowWarning(mSM->GetCString("WARN_CANT_PATH"), 3.f);
                    return ;
                }

                auto op = new ObjectPath(unit, mIsoMap, mGameMap, this);
                op->SetPath(pathMov);

                // movement failed
                if(!mGameMap->MoveUnit(op))
                {
                    delete op;
                    onFail();
                    unit->ShowWarning(mSM->GetCString("WARN_MOV_FAIL"), 2.f);
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
                    unit->ShowWarning(mSM->GetCString("WARN_CANT_BUILD"), 3.f);
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
        unit->ShowWarning(mSM->GetCString("WARN_CANT_PATH"), 3.f);
        return ;
    }

    mWallPath.reserve(mWallPath.size() + path.size());
    mWallPath.insert(mWallPath.end(), path.begin(), path.end());
}

void ScreenGame::HandleMiniUnitSetTargetOnMouseUp(GameObject * obj, const Cell2D & clickCell)
{
    using namespace sgl;

    const int clickInd = clickCell.row * mGameMap->GetNumCols() + clickCell.col;

    const Player * player = GetGame()->GetPlayerByFaction(obj->GetFaction());

    // destination is NOT visible and walkable
    if(!mLocalPlayer->IsCellVisible(clickInd) ||
       !mGameMap->IsCellWalkable(clickCell.row, clickCell.col))
    {
        obj->ShowWarning(mSM->GetCString("WARN_CELL_NOT_VALID"), 3.f);
        return ;
    }

    // find shortest path to destination checking all MiniUnits in group
    auto group = static_cast<MiniUnitsGroup *>(obj->GetGroup());

    std::vector<unsigned int> path;

    group->DoForAll([this, clickCell, &path](GameObject * o)
    {
        // init action states
        o->SetActiveAction(GameObjectActionType::MOVE);
        o->SetCurrentAction(GameObjectActionType::IDLE);

        static_cast<MiniUnit *>(o)->SetMoving(true);

        // find path to target
        const Cell2D start(o->GetRow0(), o->GetCol0());

        const auto p = mPathfinder->MakePath(start.row, start.col, clickCell.row, clickCell.col,
                                             ai::Pathfinder::NO_OPTION);

        if(path.empty() || (!p.empty() && p.size() < path.size()))
            path = std::move(p);
    });

    // can't find a valid path
    if(path.empty())
    {
        // reset active action
        group->DoForAll([](GameObject * o)
        {
            static_cast<MiniUnit *>(o)->SetMoving(false);
            o->SetActiveAction(GameObjectActionType::IDLE);
        });

        obj->ShowWarning(mSM->GetCString("WARN_CANT_PATH"), 3.f);

        return ;
    }

    mPathOverlay->SetPath(path, obj->GetFaction());

    group->SetPath(std::move(path));
    group->SetTarget(clickCell);

    // clear target indicator
    auto layerInd = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS3);
    layerInd->ClearObject(mPathIndicator);
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
    if(selObj->GetObjectCategory() == ObjectData::CAT_UNIT)
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
                            selUnit->ShowWarning(mSM->GetCString("WARN_CANT_CONQUEST"), 2.f);

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
                    selUnit->ShowWarning(mSM->GetCString("WARN_CANT_PATH"), 3.f);
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
    else if(selObj->GetObjectCategory() == ObjectData::CAT_MINI_UNIT)
    {
        const GameObjectActionType action = selObj->GetActiveAction();

        if(action == GameObjectActionType::SET_TARGET)
            HandleMiniUnitSetTargetOnMouseUp(selObj, clickCell);
    }
    else if(selObj->GetObjectType() == ObjectData::TYPE_HOSPITAL)
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

void ScreenGame::ShowActiveUnitIndicators(Unit * unit, const Cell2D & cell)
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
    else if(action == GameObjectActionType::ATTACK)
        UpdatePanelHit(unit);
}

void ScreenGame::ShowActiveMiniUnitIndicators(MiniUnit * mu, const Cell2D & cell)
{
    const GameObjectActionType action = mu->GetActiveAction();

    // only indicator is for SET TARGET
    if(action != SET_TARGET)
        return ;

    auto layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS3);

    // check if need to show indicator
    const int destInd = cell.row * mGameMap->GetNumCols() + cell.col;

    const bool showIndicator = mIsoMap->IsCellInside(cell) &&
                               mLocalPlayer->IsCellVisible(destInd) &&
                               mGameMap->IsCellWalkable(destInd);

    if(!showIndicator)
    {
        // hide the indicator, if any
        layer->SetObjectVisible(mPathIndicator, false);
        return ;
    }

    // indicator already visible
    if(layer->HasObject(mPathIndicator))
    {
        layer->MoveObject(mPathIndicator, cell.row, cell.col);
        layer->SetObjectVisible(mPathIndicator, true);
    }
    // indicator not visible yet
    else
        layer->AddObject(mPathIndicator, cell.row, cell.col);
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
    for(int i = 0; i < neededInd; ++i)
    {
        mAttIndicators[i]->SetVisible(true);
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
            {
                layer->AddObject(mAttIndicators[ind], r, c);

                const int distR = std::abs(r - r0);
                const int distC = std::abs(c - c0);
                const int dist = distR > distC ? distR : distC;

                mAttIndicators[ind]->SetDistance(dist, range);

                ++ind;
            }
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
    // cell outside the map
    if(!mIsoMap->IsCellInside(dest))
    {
        mPathOverlay->ClearPath();
        return ;
    }

    // check destination is visible
    const int destInd = dest.row * mGameMap->GetNumCols() + dest.col;
    const bool destVisible = mLocalPlayer->IsCellVisible(destInd);

    if(!destVisible)
    {
        mPathOverlay->ClearPath();
        return ;
    }

    // check if destination is not occupied or if object on it can be conquered
    const GameMapCell & destGC = mGameMap->GetCell(destInd);
    const GameObject * destObj = destGC.objTop;

    Cell2D destFinal;

    if(destObj != nullptr && !destGC.walkable)
    {
        // can't be conquered or adjacent -> no move
        if(!destObj->CanBeConquered() || mGameMap->AreObjectsAdjacent(obj, destObj))
        {
            mPathOverlay->ClearPath();
            return ;
        }

        // find a new destination next to conquerable object
        const Cell2D start(obj->GetRow0(), obj->GetCol0());
        destFinal = mGameMap->GetAdjacentMoveTarget(start, destObj);

        // failed to find a suitable target
        if(-1 == destFinal.row || -1 == destFinal.col)
        {
            mPathOverlay->ClearPath();
            return ;
        }
    }
    // no object -> use original destination
    else
        destFinal = dest;

    // show path with cost when destination is reachable
    const auto path = mPathfinder->MakePath(obj->GetRow0(), obj->GetCol0(), destFinal.row,
                                            destFinal.col, sgl::ai::Pathfinder::ALL_OPTIONS);

    ObjectPath op(obj, mIsoMap, mGameMap, this);
    op.SetPath(path);

    const int cost = op.GetPathCost();
    const int energyObj = obj->GetEnergy();
    const int energyTurn = mLocalPlayer->GetTurnEnergy();
    const bool doable = energyObj >= cost && energyTurn >= cost;

    mPathOverlay->SetPath(path, obj->GetFaction(), cost, doable);
}

void ScreenGame::ClearCellOverlays()
{
    IsoLayer * layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS2);
    layer->ClearObjects();

    layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS3);
    layer->ClearObjects();

    layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS4);
    layer->ClearObjects();
}

void ScreenGame::ClearTempStructIndicator()
{
    if(mTempStructIndicator)
    {
        IsoLayer * layer = mIsoMap->GetLayer(MapLayers::CELL_OVERLAYS4);
        layer->ClearObject(mTempStructIndicator);

        delete mTempStructIndicator;
        mTempStructIndicator = nullptr;
    }
}

void ScreenGame::UpdatePanelHit(const GameObject * attacker)
{
    if(attacker == nullptr)
    {
        mHUD->HidePanelHit();
        return;
    }

    const GameMapCell & gmCell = mGameMap->GetCell(mCurrCell.row, mCurrCell.col);
    const GameObject * objTarget = gmCell.objTop != nullptr ? gmCell.objTop :
                                   gmCell.objBottom;

    auto weapon = attacker->GetWeapon();

    if(objTarget != nullptr && objTarget != attacker && weapon->IsTargetInRange(objTarget))
        mHUD->ShowPanelHit(attacker, objTarget);
    else
        mHUD->HidePanelHit();
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

    if(sel == nullptr)
        return;

    if(sel->GetObjectCategory() == ObjectData::CAT_UNIT)
        ShowActiveUnitIndicators(static_cast<Unit *>(sel), cell);
    else if(sel->GetObjectCategory() == ObjectData::CAT_MINI_UNIT)
        ShowActiveMiniUnitIndicators(static_cast<MiniUnit *>(sel), cell);
}

void ScreenGame::AddObjectToMinimap(const Cell2D & cell, GameObjectTypeId type, PlayerFaction f)
{
    const ObjectData & data = GetGame()->GetObjectsRegistry()->GetObjectData(type);
    const auto mtype = static_cast<MiniMap::MiniMapElemType>(MiniMap::MME_FACTION1 + f);

    MiniMap * mm = mHUD->GetMinimap();
    mm->AddElement(cell.row, cell.col, data.GetRows(), data.GetCols(), mtype, f);
}

void ScreenGame::EndTurn()
{
    Game * game = GetGame();

    // END TURN
    std::cout << "ScreenGame::EndTurn - END PLAYER " << mActivePlayerIdx << "\n" << std::endl;

    // current active player is local player
    if(IsCurrentTurnLocal())
    {
        // store last local object selected before the turn ends
        mLastSelected = mLocalPlayer->GetSelectedObject();

        ClearSelection(mLocalPlayer);
    }

    // START NEW TURN
    const int players = game->GetNumPlayers();

    mActivePlayerIdx = (mActivePlayerIdx + 1) % players;

    std::cout << "ScreenGame::EndTurn - START PLAYER " << mActivePlayerIdx << std::endl;

    // reset flag for local turn init
    if(IsCurrentTurnLocal())
        mLocalTurnInitDone = false;

    // update active player data
    Player * p = game->GetPlayerByIndex(mActivePlayerIdx);
    const PlayerFaction activeFaction = p->GetFaction();

    p->ResetTurnEnergy();
    p->OnNewTurn();

    mGameMap->OnNewTurn(activeFaction);

    // new active player is local player
    if(IsCurrentTurnLocal())
    {
        mTrackerMG->AddPlayedTurns();
        mHUD->AddPlayedTurn();

        if(!mGameMap->IsDoingAutomaticMoves())
            InitLocalTurn();
    }
    // new active player is AI
    else
    {
        mHUD->SetLocalActionsEnabled(false);
        mHUD->UpdatePanelTurnControl();
    }
}

void ScreenGame::InitLocalTurn()
{
    if(mLocalTurnInitDone)
        return;

    std::cout << "ScreenGame::InitLocalTurn - turn: " << mLocalPlayer->GetTurnsPlayed() << std::endl;
    SetLocalTurnStage(TURN_STAGE_PLAY);

    mHUD->SetLocalActionsEnabled(true);

    ReselectLastSelected();

    // reset focus to Stage
    sgl::sgui::Stage::Instance()->SetFocus();

    mLocalTurnInitDone = true;
}

void ScreenGame::ReselectLastSelected()
{
    if(mLastSelected == nullptr)
        return ;

    // make sure object is still valid
    if(mGameMap->HasObject(mLastSelected))
        SelectObject(mLastSelected, mLocalPlayer);

    mLastSelected = nullptr;
}

#ifdef DEV_MODE
void ScreenGame::CreateEnemyInCurrentCell(GameObjectTypeId type)
{
    const GameMapCell & gmCell = mGameMap->GetCell(mCurrCell.row, mCurrCell.col);

    if(gmCell.objTop == nullptr && gmCell.objBottom == nullptr)
    {
        auto player = mAiPlayers[0];
        auto unit = mGameMap->CreateUnit(type, mCurrCell, player);
        player->RemoveUnit(unit);
    }
}
#endif

} // namespace game
