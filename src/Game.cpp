#include "Game.h"

#include "GameConstants.h"
#include "Planet.h"
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
#include <sgl/utilities/BinaryFile.h>
#include <sgl/utilities/StateManager.h>
#include <sgl/utilities/StringManager.h>

#ifdef DEBUG
#include <sgl/core/event/MouseEvent.h>
#include <sgl/core/ModuleCore.h>
#include <sgl/graphic/ModuleGraphic.h>
#include <sgl/media/ModuleMedia.h>

#include <iostream>
#endif

#ifdef DEV_MODE
#include <chrono>
#include <iostream>
#endif

#include <filesystem>

namespace game
{

#ifdef DEV_MODE
// this makes everything faster
bool Game::GOD_MODE = false;
#endif

const std::string Game::SAVE_VERSION("0.1.0");

Game::Game(int argc, char * argv[])
    : sgl::core::Application(argc, argv)
    , mTutMan(new TutorialManager)
    , mObjsRegistry(new ObjectsDataRegistry)
    , mLocalFaction(NO_FACTION)
    , mCurrPlanet(PLANET_UNKNOWN)
    , mLanguage(LANG_NULL)
    , mButtonSelect(sgl::core::MouseEvent::BUTTON_LEFT)
    , mButtonAction(sgl::core::MouseEvent::BUTTON_RIGHT)
#ifdef DEV_MODE
    // tutorial disabled in DEV MODE
    //, mTutorialEnabled(false)
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

#ifdef DEV_MODE
    mTimeAutoHideMouse = 1.f;
#endif

    mStage->AutoHideInactiveCursor(true, mTimeAutoHideMouse);

    // INIT TECH UPGRADES COST
    mCostUpgrades.emplace(TECH_UP_NULL, 0);
    mCostUpgrades.emplace(TECH_UP_BASE_IMPROVE_1, 500);
    mCostUpgrades.emplace(TECH_UP_BASE_IMPROVE_2, 2000);
    mCostUpgrades.emplace(TECH_UP_BASE_IMPROVE_3, 4000);
    mCostUpgrades.emplace(TECH_UP_BASE_IMPROVE_4, 6500);
    mCostUpgrades.emplace(TECH_UP_BASE_IMPROVE_5, 9500);
    mCostUpgrades.emplace(TECH_UP_RADAR_STATION, 1000);
    mCostUpgrades.emplace(TECH_UP_RADAR_TOWER, 1000);
    mCostUpgrades.emplace(TECH_UP_STORAGE_STRUCTS, 2000);
    mCostUpgrades.emplace(TECH_UP_STORAGE_ENERGY_1, 3000);
    mCostUpgrades.emplace(TECH_UP_STORAGE_ENERGY_2, 6000);
    mCostUpgrades.emplace(TECH_UP_STORAGE_MATERIAL_1, 3000);
    mCostUpgrades.emplace(TECH_UP_STORAGE_MATERIAL_2, 6000);
    mCostUpgrades.emplace(TECH_UP_STORAGE_DIAMONDS_1, 3000);
    mCostUpgrades.emplace(TECH_UP_STORAGE_DIAMONDS_2, 6000);
    mCostUpgrades.emplace(TECH_UP_STORAGE_BLOBS_1, 3000);
    mCostUpgrades.emplace(TECH_UP_STORAGE_BLOBS_2, 6000);
    mCostUpgrades.emplace(TECH_UP_PRACTICE_TARGET, 1000);
    mCostUpgrades.emplace(TECH_UP_TRADING_POST, 1000);
    mCostUpgrades.emplace(TECH_UP_UNIT_SLOTS_1, 1500);
    mCostUpgrades.emplace(TECH_UP_UNIT_SLOTS_2, 3000);
    mCostUpgrades.emplace(TECH_UP_UNIT_SLOTS_3, 5000);
    mCostUpgrades.emplace(TECH_UP_UNIT_SLOTS_4, 8000);
    mCostUpgrades.emplace(TECH_UP_UNIT_SLOTS_5, 12000);

    // TEMP CODE
    // TODO handle save directory properly
    mDirSave = "save/";

    InitDirectories();

    // TODO handle save files properly
    mCurrSaveFile = mDirSave + std::string("001.sav");
}

Game::~Game()
{
    using namespace sgl;

    for(auto it: mCursors)
        delete it.second;

    delete mTutMan;

    // delete states and screens
    delete mStateMan;

    delete mObjsRegistry;

    ClearPlanets();
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
    Planet * planet = nullptr;

    // -- MAPS --
    // PLANET 1
    planet = new Planet(PLANET_1, PLANET_SIZE_S);
#ifdef DEV_MODE
    planet->AddMap("data/maps/01-01.map", NO_FACTION, TER_ST_UNEXPLORED);
#else
    planet->AddMap("data/maps/01-01.map", NO_FACTION, TER_ST_UNEXPLORED);
#endif
    planet->AddMap("data/maps/60x60-01.map", NO_FACTION, TER_ST_UNEXPLORED);
    planet->AddMap("data/maps/01-02.map", NO_FACTION, TER_ST_UNREACHABLE);
    planet->AddMap("data/maps/80x80-01.map", NO_FACTION, TER_ST_UNREACHABLE);
    planet->AddMap("data/maps/01-03.map", NO_FACTION, TER_ST_UNREACHABLE);

    mPlanets.emplace(PLANET_1, planet);
}

void Game::ClearGameData()
{
    ClearPlanets();

    ClearPlayers();
}

// -- LOAD & SAVE --
bool Game::SaveGame()
{
    using namespace sgl;

#ifdef DEV_MODE
    // TODO remove later, now left just for reference on testing times
    auto t0 = std::chrono::high_resolution_clock::now();
    std::cout << "Game::SaveGame - START SAVING: " << mCurrSaveFile << std::endl;
#endif

    bool res = false;

    // OPEN map file
    utilities::BinaryFile bf(mCurrSaveFile, utilities::BinaryFile::OPEN_OUTPUT, true);

    if(!bf.IsOpen())
    {
        std::cout << "[ERR] Game::SaveGame - can't open file " << mCurrSaveFile << std::endl;
        return false;
    }

    // version
    bf.WriteString(SAVE_VERSION);

    // number of Players
    const unsigned int numPlayers = mPlayers.size();
    bf.WriteUint(numPlayers);

    for(Player * p : mPlayers)
        p->Save(bf);

    // CLOSE map file
    bf.Close();

#ifdef DEV_MODE
    // TODO remove later, now left just for reference on testing times
    auto t1 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    std::cout << "Game::SaveGame - GAME SAVED in: " << duration.count() << " ms" << std::endl;
#endif

    return res;
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

Planet * Game::GetPlanet(PlanetId planetId) const
{
    auto it = mPlanets.find(planetId);

    if(it != mPlanets.end())
        return it->second;
    else
        return nullptr;
}

const std::string & Game::GetCurrentMapFile() const
{
    static const std::string empty;

    auto it = mPlanets.find(mCurrPlanet);

    if(it != mPlanets.end())
        return it->second->GetMapFile(mCurrTerritory);
    else
        return empty;
}

Planet * Game::GetCurrentPlanet() const
{
    auto it = mPlanets.find(mCurrPlanet);

    if(it != mPlanets.end())
        return it->second;
    else
        return nullptr;
}

int Game::GetResourcePriceBuy(ExtendedResource t) const
{
    // TODO make it change depending on territory/planet
    const int price[] =
    {
        60,
        70,
        120,
        170,
        1,
        250,
    };

    static_assert(sizeof(price) / sizeof(int) == NUM_EXTENDED_RESOURCES);

    if(t < NUM_EXTENDED_RESOURCES)
        return price[t];
    else
        return 0;
}

int Game::GetResourcePriceSell(ExtendedResource t) const
{
    // TODO make it change depending on territory/planet
    const int price[] =
    {
        50,
        60,
        100,
        150,
        1,
        175,
    };

    static_assert(sizeof(price) / sizeof(int) == NUM_EXTENDED_RESOURCES);

    if(t < NUM_EXTENDED_RESOURCES)
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

// -- LOAD & SAVE --
void Game::InitDirectories()
{
    using namespace std;

    // create SAVE directory if missing
    filesystem::path pathSave(mDirSave);

    if(!filesystem::exists(pathSave))
        filesystem::create_directories(pathSave);
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

void Game::ClearPlanets()
{
    for(auto it : mPlanets)
        delete it.second;

    mPlanets.clear();
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
        "fr.txt",
        "de.txt",
        "it.txt",
        "es.txt",
    };

    static_assert((sizeof(languages) / sizeof(char *)) == NUM_LANGUAGES);

    sm->LoadStringsFromPackage(languages[lang]);
}

} // namespace game
