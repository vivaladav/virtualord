#include "Game.h"

#include "GameConstants.h"
#include "MapsRegistry.h"
#include "Player.h"
#include "Version.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "States/StatesIds.h"
#include "States/StateFactionSelection.h"
#include "States/StateGame.h"
#include "States/StateInit.h"
#include "States/StateMainMenu.h"
#include "States/StateNewGame.h"
#include "States/StatePlanetMap.h"
#include "States/StateSettings.h"
#include "States/StateTest.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/GraphicConstants.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Renderer.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/graphic/Window.h>
#include <sgl/media/AudioManager.h>
#include <sgl/sgui/Stage.h>
#include <sgl/utilities/StateManager.h>

namespace game
{

Game::Game(int argc, char * argv[])
    : sgl::core::Application(argc, argv)
    , mMapsReg(new MapsRegistry)
    , mObjsRegistry(new ObjectsDataRegistry)
    , mLocalFaction(NO_FACTION)
    , mCurrPlanet(PLANET_UNKNOWN)
#ifdef DEV_MODE
    // tutorial disabled in DEV MODE
    , mTutorialEnabled(false)
#endif
{
    using namespace sgl::graphic;

    const std::string title = std::string("Virtueror - v. ") + std::string(VERSION);
    mWin = Window::Create(title.c_str(), 0, 0, this);
    mRenderer = Renderer::Create(mWin, true);
    mRenderer->SetLogicalSize(1920, 1080);
    mWin->SetVideoMode(sgl::graphic::Window::VM_FULLSCREEN);

    TextureManager::Instance()->SetNewTextureQuality(TextureQuality::BEST);

    FontManager::Create();

    // -- State Manager --
    mStateMan = new sgl::utilities::StateManager;

    mStateMan->AddState(new StateFactionSelection(this));
    mStateMan->AddState(new StateGame(this));
    mStateMan->AddState(new StateInit(this));
    mStateMan->AddState(new StateMainMenu(this));
    mStateMan->AddState(new StateNewGame(this));
    mStateMan->AddState(new StatePlanetMap(this));
    mStateMan->AddState(new StateSettings(this));
    mStateMan->AddState(new StateTest(this));

    mStateMan->RequestNextActiveState(StateId::INIT);

    // -- AUDIO --
    const int defVolumeMusic = 50;
    const int defVolumeSound = 50;

    mAudioMan = sgl::media::AudioManager::Create();
    mAudioMan->SetVolumeMusic(defVolumeMusic);
    mAudioMan->SetVolumeSound(defVolumeSound);

    // -- SGUI Stage --
    mStage = sgl::sgui::Stage::Create();
    AddKeyboardListener(mStage);
    AddMouseListener(mStage);
}

Game::~Game()
{
    // delete states and screens
    delete mStateMan;

    delete mMapsReg;
    delete mObjsRegistry;

    ClearPlayers();

    sgl::sgui::Stage::Destroy();

    sgl::media::AudioManager::Destroy();

    sgl::graphic::FontManager::Destroy();

    sgl::graphic::Renderer::Destroy();
    sgl::graphic::Window::Destroy();
}

void Game::InitGameData()
{
    // -- MAPS --
    // PLANET 1
    mMapsReg->CreatePlanet(PLANET_1);
    //               planetId, file, energy, material, diamonds, blobs, size, value, occupier, status
    mMapsReg->AddMap(PLANET_1, "data/maps/80x80-01.map", 5, 4, 2, 2, 80, 3, NO_FACTION, TER_ST_UNEXPLORED, MISSION_DESTROY_ENEMY_BASE);
    mMapsReg->AddMap(PLANET_1, "data/maps/40x40-01.map", 2, 2, 1, 1, 40, 2, NO_FACTION, TER_ST_UNEXPLORED, MISSION_RESIST_TIME);
    mMapsReg->AddMap(PLANET_1, "data/maps/60x60-01.map", 4, 4, 2, 2, 60, 3, NO_FACTION, TER_ST_UNREACHABLE, MISSION_DESTROY_ENEMY_BASE);
    mMapsReg->AddMap(PLANET_1, "data/maps/20x20-empty.map", 1, 2, 1, 1, 20, 1, NO_FACTION, TER_ST_UNREACHABLE, MISSION_DESTROY_ENEMY_BASE);
    mMapsReg->AddMap(PLANET_1, "data/maps/80x80-01.map", 5, 4, 2, 2, 80, 3, NO_FACTION, TER_ST_UNREACHABLE, MISSION_DESTROY_ENEMY_BASE);
    mMapsReg->AddMap(PLANET_1, "data/maps/40x40-01.map", 2, 2, 1, 1, 40, 2, NO_FACTION, TER_ST_UNREACHABLE, MISSION_DESTROY_ENEMY_BASE);
    mMapsReg->AddMap(PLANET_1, "data/maps/60x60-01.map", 4, 4, 2, 2, 60, 3, NO_FACTION, TER_ST_UNREACHABLE, MISSION_DESTROY_ENEMY_BASE);
    mMapsReg->AddMap(PLANET_1, "data/maps/20x20-empty.map", 1, 2, 1, 1, 20, 1, NO_FACTION, TER_ST_UNREACHABLE, MISSION_DESTROY_ENEMY_BASE);
    mMapsReg->AddMap(PLANET_1, "data/maps/20x20-empty.map", 1, 2, 1, 1, 20, 1, NO_FACTION, TER_ST_UNREACHABLE, MISSION_DESTROY_ENEMY_BASE);
    mMapsReg->AddMap(PLANET_1, "data/maps/40x40-01.map", 2, 2, 1, 1, 40, 2, NO_FACTION, TER_ST_UNREACHABLE, MISSION_DESTROY_ENEMY_BASE);
    mMapsReg->AddMap(PLANET_1, "data/maps/60x60-01.map", 4, 4, 2, 2, 60, 3, NO_FACTION, TER_ST_UNEXPLORED, MISSION_DESTROY_ALL_ENEMIES);
    mMapsReg->AddMap(PLANET_1, "data/maps/80x80-01.map", 5, 4, 2, 2, 80, 3, NO_FACTION, TER_ST_UNEXPLORED, MISSION_DESTROY_ENEMY_BASE);
}

void Game::ClearGameData()
{
    mMapsReg->ClearData();

    ClearPlayers();
}

const std::string & Game::GetCurrentMapFile() const
{
    return mMapsReg->GetMapFile(mCurrPlanet, mCurrTerritory);
}

void Game::RequestNextActiveState(StateId sid) { mStateMan->RequestNextActiveState(sid); }

void Game::SetMapDraggingSpeed(int val)
{
    const int minSpeed = 1;
    const int maxSpeed = 10;

    if(val < minSpeed)
        val = minSpeed;
    else if(val > maxSpeed)
        val = maxSpeed;

    mMapDraggingSpeed = val;

    NotifyOnSettingsChanged();
}

void Game::SetMapScrollingSpeed(int val)
{
    const int minSpeed = 1;
    const int maxSpeed = 10;

    if(val < minSpeed)
        val = minSpeed;
    else if(val > maxSpeed)
        val = maxSpeed;

    mMapScrollingSpeed = val;

    NotifyOnSettingsChanged();
}

unsigned int Game::AddOnSettingsChangedFunction(const std::function<void()> & f)
{
    static unsigned int num = 0;

    const unsigned int fId = ++num;
    mOnSettingsChanged.emplace(fId, f);

    return fId;
}

void Game::RemoveOnSettingsChangedFunction(unsigned int fId)
{
    auto it = mOnSettingsChanged.find(fId);

    if(it != mOnSettingsChanged.end())
        mOnSettingsChanged.erase(it);
}

void Game::NotifyOnSettingsChanged()
{
    for(auto & it: mOnSettingsChanged)
        it.second();
}

void Game::Update(float delta)
{
    mRenderer->Clear(mClearR, mClearG, mClearB, mClearA);

    mStateMan->UpdateActive();

    auto * state = static_cast<BaseGameState *>(mStateMan->GetActiveState());

    state->Update(delta);
    mStage->Update(delta);
    mAudioMan->Update(delta);

    state->Render();
    mStage->Render();

    mRenderer->Finalize();
}

Player * Game::AddPlayer(const char * name, int pid)
{
    if(mPlayers.size() == MAX_NUM_PLAYERS)
        return nullptr;

    Player * p = new Player(name, pid);

    mPlayers.push_back(p);

    return p;
}

void Game::ClearPlayers()
{
    for(Player * p : mPlayers)
        delete p;

    mPlayers.clear();
}


Player * Game::GetPlayerByFaction(PlayerFaction faction) const
{
    for(Player * p : mPlayers)
    {
        if(p->GetFaction() == faction)
            return p;
    }

    return nullptr;
}

} // namespace game
