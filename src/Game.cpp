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
#include "States/StateInitGame.h"
#include "States/StateLeaveGame.h"
#include "States/StateLeavePregame.h"
#include "States/StateMainMenu.h"
#include "States/StateNewGame.h"
#include "States/StatePlanetMap.h"
#include "States/StateSettings.h"
#include "States/StateTest.h"
#include "Tutorial/TutorialManager.h"

#include <sgl/graphic/Cursor.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/GraphicConstants.h>
#include <sgl/graphic/Renderer.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/graphic/Window.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/Stage.h>
#include <sgl/utilities/StateManager.h>
#include <sgl/utilities/StringManager.h>

#ifdef DEBUG
#include <sgl/core/ModuleCore.h>
#include <sgl/graphic/ModuleGraphic.h>
#include <sgl/media/ModuleMedia.h>

#include <iostream>
#endif

namespace game
{

#ifdef DEV_MODE
// this makes everything faster
bool Game::GOD_MODE = false;
#endif

Game::Game(int argc, char * argv[])
    : sgl::core::Application(argc, argv)
    , mTutMan(new TutorialManager)
    , mMapsReg(new MapsRegistry)
    , mObjsRegistry(new ObjectsDataRegistry)
    , mLocalFaction(NO_FACTION)
    , mCurrPlanet(PLANET_UNKNOWN)
    , mLanguage(LANG_NULL)
#ifdef DEV_MODE
    // tutorial disabled in DEV MODE
    , mTutorialEnabled(false)
#endif
{
    using namespace sgl;

#ifdef DEBUG
    // -- build libraries --
    core::ModuleCore::PrintBuildLibs();
    graphic::ModuleGraphic::PrintBuildLibs();
    media::ModuleMedia::PrintBuildLibs();

    std::cout << std::endl;

    // -- runtime libraries --
    core::ModuleCore::PrintRuntimeLibs();
    graphic::ModuleGraphic::PrintRuntimeLibs();
    media::ModuleMedia::PrintRuntimeLibs();

    std::cout << std::endl;
#endif

    // -- INIT TEXT DATA --
    auto sm = utilities::StringManager::Create();
    sm->RegisterPackage("data/text/game.bin");

    SetLanguage(LANG_ENGLISH);

    const std::string title = std::string("Virtualord - v. ") + std::string(VERSION);
    mWin = graphic::Window::Create(title.c_str(), 0, 0, this);
    mRenderer = graphic::Renderer::Create(mWin, true);
    mRenderer->SetLogicalSize(1920, 1080);
    mWin->SetVideoMode(graphic::Window::VM_BORDERLESS);

    graphic::TextureManager::Instance()->SetNewTextureQuality(graphic::TextureQuality::BEST);

    graphic::FontManager::Create();

#ifdef DEBUG
    graphic::ModuleGraphic::PrintVideoInfo();
    std::cout << std::endl;
#endif

    // -- State Manager --
    mStateMan = new utilities::StateManager;

    mStateMan->AddState(new StateFactionSelection(this));
    mStateMan->AddState(new StateGame(this));
    mStateMan->AddState(new StateInit(this));
    mStateMan->AddState(new StateInitGame(this));
    mStateMan->AddState(new StateLeaveGame(this));
    mStateMan->AddState(new StateLeavePregame(this));
    mStateMan->AddState(new StateMainMenu(this));
    mStateMan->AddState(new StateNewGame(this));
    mStateMan->AddState(new StatePlanetMap(this));
    mStateMan->AddState(new StateSettings(this));
    mStateMan->AddState(new StateTest(this));

    mStateMan->SetInitialActiveState(StateId::INIT);

    // -- AUDIO --
    const int defVolumeMusic = 50;
    const int defVolumeSound = 50;

    mAudioMan = media::AudioManager::Create();
    mAudioMan->SetVolumeMusic(defVolumeMusic);
    mAudioMan->SetVolumeSound(defVolumeSound);

#ifdef DEV_MODE
    // music OFF by default when developer
    mAudioMan->GetPlayer()->SetMusicEnabled(false);
#endif

    // -- SGUI Stage --
    mStage = sgui::Stage::Create();
    AddKeyboardListener(mStage);
    AddMouseListener(mStage);
}

Game::~Game()
{
    using namespace sgl;

    for(auto it: mCursors)
        delete it.second;

    delete mTutMan;

    // delete states and screens
    delete mStateMan;

    delete mMapsReg;
    delete mObjsRegistry;

    ClearPlayers();

    sgui::Stage::Destroy();

    media::AudioManager::Destroy();

    graphic::FontManager::Destroy();

    graphic::Renderer::Destroy();
    graphic::Window::Destroy();

    utilities::StringManager::Destroy();
}

void Game::InitGameData()
{
    // -- MAPS --
    // PLANET 1
    mMapsReg->CreatePlanet(PLANET_1);
    //               planetId, file, occupier, status
    mMapsReg->AddMap(PLANET_1, "data/maps/80x80-01.map", NO_FACTION, TER_ST_UNEXPLORED);
    mMapsReg->AddMap(PLANET_1, "data/maps/40x40-01.map", NO_FACTION, TER_ST_UNEXPLORED);
    mMapsReg->AddMap(PLANET_1, "data/maps/60x60-01.map", NO_FACTION, TER_ST_UNREACHABLE);
    mMapsReg->AddMap(PLANET_1, "data/maps/20x20-empty.map", NO_FACTION, TER_ST_UNREACHABLE);
    mMapsReg->AddMap(PLANET_1, "data/maps/80x80-01.map", NO_FACTION, TER_ST_UNREACHABLE);
    mMapsReg->AddMap(PLANET_1, "data/maps/40x40-01.map", NO_FACTION, TER_ST_UNREACHABLE);
    mMapsReg->AddMap(PLANET_1, "data/maps/60x60-01.map", NO_FACTION, TER_ST_UNREACHABLE);
    mMapsReg->AddMap(PLANET_1, "data/maps/20x20-empty.map", NO_FACTION, TER_ST_UNREACHABLE);
    mMapsReg->AddMap(PLANET_1, "data/maps/80x80-01.map", NO_FACTION, TER_ST_UNREACHABLE);

    // -- UPGRADES --
    mUpgrades.emplace(TECH_UP_NULL, false);
    mUpgrades.emplace(TECH_UP_BASE_IMPROVE_1, false);
    mUpgrades.emplace(TECH_UP_BASE_IMPROVE_2, false);
    mUpgrades.emplace(TECH_UP_BASE_IMPROVE_3, false);
    mUpgrades.emplace(TECH_UP_BASE_IMPROVE_4, false);
    mUpgrades.emplace(TECH_UP_BASE_IMPROVE_5, false);
}

void Game::ClearGameData()
{
    mMapsReg->ClearData();
    ClearPlayers();
    ClearUpgrades();
}

// -- mouse cursors --
void Game::RegisterCursor(GameCursorId curId, sgl::graphic::Cursor * cursor)
{
    mCursors.emplace(curId, cursor);
}

void Game::SetCurrentCursor(GameCursorId curId)
{
    const auto it = mCursors.find(curId);

    if(it == mCursors.end())
        return ;

    sgl::sgui::Stage::Instance()->SetCursor(it->second);
}

const std::string & Game::GetCurrentMapFile() const
{
    return mMapsReg->GetMapFile(mCurrPlanet, mCurrTerritory);
}

int Game::GetResourcePriceBuy(ResourceType t) const
{
    // TODO make it change depending on territory/planet

    const int price[NUM_RESOURCES] =
    {
        60,
        70,
        120,
        170
    };

    if(t < NUM_RESOURCES)
        return price[t];
    else
        return 0;
}

int Game::GetResourcePriceSell(ResourceType t) const
{
    // TODO make it change depending on territory/planet

    const int price[NUM_RESOURCES] =
    {
        50,
        60,
        100,
        150
    };

    if(t < NUM_RESOURCES)
        return price[t];
    else
        return 0;
}

int Game::GetActiveStateId() const { return mStateMan->GetActiveStateId(); }

void Game::RequestNextActiveState(StateId sid, sgl::utilities::StateData * data)
{
    mStateMan->RequestNextActiveState(sid, data);
}

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

    // UPDATE
    mStateMan->Update(delta);
    mStage->Update(delta);
    mAudioMan->Update(delta);

    // RENDER
    auto * state = static_cast<BaseGameState *>(mStateMan->GetActiveState());
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

void Game::SetLanguage(LanguageId lang)
{
    if(lang == mLanguage)
        return ;

    mLanguage = lang;

    auto sm = sgl::utilities::StringManager::Instance();

    const char * languages[] =
    {
        "en.txt",
        "it.txt",
        "es.txt",
    };

    sm->LoadStringsFromPackage(languages[lang]);
}

} // namespace game
