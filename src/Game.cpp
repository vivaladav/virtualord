#include "Game.h"

#include "GameConstants.h"
#include "GameStats.h"
#include "Planet.h"
#include "Player.h"
#include "ResourceLoader.h"
#include "Version.h"
#include "AI/PlayerAI.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "States/StatesIds.h"
#include "States/StateCredits.h"
#include "States/StateFactionSelection.h"
#include "States/StateGame.h"
#include "States/StateInit.h"
#include "States/StateInitGame.h"
#include "States/StateInitGameSetup.h"
#include "States/StateLoadGame.h"
#include "States/StateMainMenu.h"
#include "States/StateNewGameSetup.h"
#include "States/StatePlanetMap.h"
#include "States/StateSettings.h"
#include "States/StateTest.h"
#include "Tutorial/TutorialManager.h"

#include <sgl/core/event/MouseEvent.h>
#include <sgl/graphic/Cursor.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/GraphicConstants.h>
#include <sgl/graphic/ModuleGraphic.h>
#include <sgl/graphic/Renderer.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/graphic/Window.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/services/Steam.h>
#include <sgl/sgui/Stage.h>
#include <sgl/utilities/BinaryFile.h>
#include <sgl/utilities/StateManager.h>
#include <sgl/utilities/StringManager.h>
#include <sgl/utilities/UniformDistribution.h>

#ifdef DEBUG
#include <sgl/core/ModuleCore.h>
#include <sgl/media/ModuleMedia.h>
#endif

#ifdef DEV_MODE
#include <chrono>
#include <sstream>
#endif

#include <iostream>
#include <filesystem>

#include <cassert>

namespace
{
constexpr int MAX_UNITS0 = 5;

// resources when starting a new game
constexpr int START_ENERGY = 1000;
constexpr int START_MATERIAL = 1000;
constexpr int START_MONEY = 1000;
constexpr int START_DIAMONDS = 10;
constexpr int START_BLOBS = 10;
constexpr int START_RESEARCH = 0;
}

namespace game
{

#ifdef DEV_MODE
// this makes everything faster
bool Game::GOD_MODE = false;
#endif

const std::string Game::SAVE_VERSION("0.2.3");
const std::string Game::SETTINGS_VERSION("0.1.5");

Game::Game(int argc, char * argv[])
    : sgl::core::Application(argc, argv)
    , mResLoader(new ResourceLoader(this))
    , mTutMan(new TutorialManager)
    , mObjsRegistry(new ObjectsDataRegistry)
    , mLocalFaction(NO_FACTION)
    , mCurrPlanet(PLANET_UNKNOWN)
    , mLanguage(LANG_NULL)
    , mButtonSelect(sgl::core::MouseEvent::BUTTON_LEFT)
    , mButtonAction(sgl::core::MouseEvent::BUTTON_RIGHT)
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

    // -- DISPLAY --
    std::cout << "\n===== DISPLAYS INFO =====\n";
    const int numDisplays = graphic::ModuleGraphic::GetNumDisplays();
    std::cout << "DISPLAYS: " << numDisplays << std::endl;

#ifdef DEV_MODE
    if(numDisplays > 0)
    {
        for(int d = 0; d < numDisplays; ++d)
        {
            int dX, dY, dW, dH;
            graphic::ModuleGraphic::GetDisplayBounds(d, dX, dY, dW, dH);

            std::cout << "DISPLAY " << d
                      << "(" << dX << ", " << dY << " - " << dW << " x " << dH
                      << ") - #MODES: "
                      << graphic::ModuleGraphic::GetNumDisplayModes(d)<< std::endl;

            graphic::ModuleGraphic::PrintDisplayModes(d);
        }
    }
#endif
    std::cout << "=========================\n";

    std::cout << std::endl;
#endif

    // set default language
    LanguageId startLanguage = LANG_ENGLISH;

    // -- STEAM INTEGRATION --
    auto steam = sgl::services::Steam::Create();

    const bool steamOK = steam->Init();
#ifdef DEBUG
    std::cout << "Steam init: " << (steamOK ? "OK" : "FAILED") << std::endl;
#endif

    if(steamOK)
    {
        // user info
#ifdef DEBUG
        std::cout << "Steam user: " << steam->GetUserName()
                  << " (" << steam->GetUserId() << ")" << std::endl;

        std::cout << "Steam app: " << steam->GetAppId() << std::endl;
#endif

        // update default language based on Steam settings
        const char * lang = steam->GetAppLanguage();

        if(lang != nullptr)
        {
#ifdef DEBUG
            std::cout << "Steam language: " << lang << std::endl;
#endif
            const std::string strLang(lang);

            const std::string strFrench("french");
            const std::string strGerman("german");
            const std::string strItalian("italian");
            const std::string strSpanishEs("spanish");
            const std::string strSpanishLa("latam");

            if(strLang == strSpanishEs || strLang == strSpanishLa)
                startLanguage = LANG_SPANISH;
            else if(strLang == strGerman)
                startLanguage = LANG_GERMAN;
            else if(strLang == strItalian)
                startLanguage = LANG_ITALIAN;
            else if(strLang == strFrench)
                startLanguage = LANG_FRENCH;
        }
    }

    // INIT GAME STATS
    mGameStats = new GameStats;

    // -- INIT TEXT DATA --
    auto sm = utilities::StringManager::Create();
    sm->RegisterPackage("data/text/game.bin");

    const std::string title = std::string("Virtualord - v. ") + std::string(VERSION);

    // try to create a 16:9 window
    if(graphic::ModuleGraphic::IsDisplayModeSupported(0, 1920, 1080, -1))
        mWin = graphic::Window::Create(title.c_str(), 1920, 1080, this);
    else if(graphic::ModuleGraphic::IsDisplayModeSupported(0, 1600, 900, -1))
        mWin = graphic::Window::Create(title.c_str(), 1600, 900, this);
    else if(graphic::ModuleGraphic::IsDisplayModeSupported(0, 1366, 768, -1))
        mWin = graphic::Window::Create(title.c_str(), 1366, 768, this);
    else if(graphic::ModuleGraphic::IsDisplayModeSupported(0, 1280, 720, -1))
        mWin = graphic::Window::Create(title.c_str(), 1280, 720, this);
    // all failed -> fallback to first available display whatever it is
    else
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

    mStateMan->AddState(new StateNewGameSetup(this));
    mStateMan->AddState(new StateFactionSelection(this));
    mStateMan->AddState(new StateGame(this));
    mStateMan->AddState(new StateInit(this));
    mStateMan->AddState(new StateInitGame(this));
    mStateMan->AddState(new StateInitGameSetup(this));
    mStateMan->AddState(new StateMainMenu(this));
    mStateMan->AddState(new StateLoadGame(this));
    mStateMan->AddState(new StatePlanetMap(this));
    mStateMan->AddState(new StateSettings(this));
    mStateMan->AddState(new StateCredits(this));
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

    SetAutoHideMouseEnabled(true);

    // INIT TECH UPGRADES COST
    mCostUpgrades.emplace(TECH_UP_NULL, 0);
    mCostUpgrades.emplace(TECH_UP_BASE_IMPROVE_1, 500);
    mCostUpgrades.emplace(TECH_UP_BASE_IMPROVE_2, 2000);
    mCostUpgrades.emplace(TECH_UP_BASE_IMPROVE_3, 4000);
    mCostUpgrades.emplace(TECH_UP_BASE_IMPROVE_4, 7000);
    mCostUpgrades.emplace(TECH_UP_BASE_IMPROVE_5, 11000);
    mCostUpgrades.emplace(TECH_UP_RADAR_STATION, 1000);
    mCostUpgrades.emplace(TECH_UP_RADAR_TOWER, 1000);
    mCostUpgrades.emplace(TECH_UP_STORAGE_STRUCTS, 1500);
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
    mCostUpgrades.emplace(TECH_UP_UNIT_SLOTS_1, 2000);
    mCostUpgrades.emplace(TECH_UP_UNIT_SLOTS_2, 3000);
    mCostUpgrades.emplace(TECH_UP_UNIT_SLOTS_3, 5000);
    mCostUpgrades.emplace(TECH_UP_UNIT_SLOTS_4, 8000);
    mCostUpgrades.emplace(TECH_UP_UNIT_SLOTS_5, 12000);

    // TEMP CODE
    // TODO handle save directory properly
    mDirSave = "save/";
    mDirSettings = "save/";
    InitDirectories();

    mSettingsFile = std::string("settings.sav");

    // init save files
    mSaveFiles.emplace_back("00_001.sav");
    mSaveFiles.emplace_back("00_002.sav");
    mSaveFiles.emplace_back("00_003.sav");
    mSaveFiles.emplace_back("01_001.sav");
    mSaveFiles.emplace_back("01_002.sav");
    mSaveFiles.emplace_back("01_003.sav");
    mSaveFiles.emplace_back("02_001.sav");
    mSaveFiles.emplace_back("02_002.sav");
    mSaveFiles.emplace_back("02_003.sav");

    // load settings when starting
    const bool settingsOK = LoadSettings();

    // default init when settings are not loaded (first start)
    if(!settingsOK)
    {
        SetLanguage(startLanguage);
    }
}

Game::~Game()
{
    using namespace sgl;

    delete mGameStats;

    sgl::services::Steam::Destroy();

    for(auto it: mCursors)
        delete it.second;

    delete mTutMan;

    // delete states and screens
    delete mStateMan;

    delete mObjsRegistry;

    ClearPlanets();
    ClearPlayers();

    delete mResLoader;

    CloseSaveFileForReading();

    sgui::Stage::Destroy();

    media::AudioManager::Destroy();

    graphic::FontManager::Destroy();

    graphic::Renderer::Destroy();
    graphic::Window::Destroy();

    utilities::StringManager::Destroy();
}

void Game::InitNewGameData()
{
    CreatePlayers();

    CreatePlanets();

    InitCostSaveGame();
}

void Game::ClearGameData()
{
    ClearPlanets();

    ClearPlayers();

    mTutMan->ResetTutorialState();
}

// -- LOAD & SAVE --
void Game::CloseSaveFileForReading()
{
    if(mReaderSave == nullptr)
        return ;

    mReaderSave->Close();

    delete mReaderSave;
    mReaderSave = nullptr;
}

const std::string & Game::GetSavefile(GameMode mode, unsigned int ind) const
{
    static const std::string empty;

    const unsigned int savesPerMode = 3;

    if(mode < NUM_GAME_MODES && ind < savesPerMode)
        return mSaveFiles[(savesPerMode * mode) + ind];
    else
        return empty;
}

bool Game::HasSavedFiles() const
{
    for(const std::string & f : mSaveFiles)
    {
        if(IsSaveFileValid(f))
            return true;
    }

    return false;
}

bool Game::IsSaveFileValid(const std::string & saveFile) const
{
    using namespace sgl;

    // OPEN save file
    const std::string path = saveFile.empty() ? (mDirSave + mCurrSaveFile) : (mDirSave + saveFile);

    utilities::BinaryFile bf(path, utilities::BinaryFile::OPEN_INPUT);

    if(!bf.IsOpen())
    {
        std::cout << "[ERR] Game::IsSaveFileValid - can't open file " << path << std::endl;
        return false;
    }

    // version
    std::string version;
    bf.ReadString(version);

    if(version != SAVE_VERSION)
    {
        std::cout << "[ERR] Game::IsSaveFileValid - version in file " << path << " is "
                  << version << " (expected " << SAVE_VERSION << ")" << std::endl;
        return false;
    }

    // CLOSE save file
    bf.Close();

    return true;
}

bool Game::LoadGameHeader(GameMode & mode, PlayerFaction & faction, PlanetId & planet,
                          unsigned int & map, int & turn, std::time_t & timestamp,
                          const std::string & saveFile)
{
    using namespace sgl;

    const std::string path = saveFile.empty() ? (mDirSave + mCurrSaveFile) : (mDirSave + saveFile);

#ifdef DEV_MODE
    // TODO remove later, now left just for reference on testing times
    std::cout << "Game::LoadGameHeader - START LOADING: " << path << std::endl;
    auto t0 = std::chrono::high_resolution_clock::now();
#endif

    // OPEN save file for reading
    utilities::BinaryFile bf(path, utilities::BinaryFile::OPEN_INPUT);

    if(!bf.IsOpen())
    {
        std::cout << "[ERR] Game::LoadGameHeader - can't open file " << path << std::endl;

        return false;
    }

    // VERSION
    std::string version;
    bf.ReadString(version);

    if(version != SAVE_VERSION)
    {
        std::cout << "[ERR] Game::LoadGameHeader - version in file " << path << " is "
                  << version << " (expected " << SAVE_VERSION << ")" << std::endl;
        return false;
    }

    // READ HEADER
    mode = static_cast<GameMode>(bf.ReadUint());
    faction = static_cast<PlayerFaction>(bf.ReadUint());
    planet = static_cast<PlanetId>(bf.ReadUint());
    map = bf.ReadUint();
    turn = bf.ReadInt();
    timestamp = bf.ReadTimeT();

#ifdef DEV_MODE
    // TODO remove later, now left just for reference on testing times
    auto t1 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    std::cout << "Game::LoadGameHeader - GAME HEADER LOADED in: " << duration.count() << " ms" << std::endl;
#endif

    return true;
}

bool Game::LoadGame(const std::string & saveFile)
{
    using namespace sgl;

    const std::string path = saveFile.empty() ? (mDirSave + mCurrSaveFile) : (mDirSave + saveFile);

#ifdef DEV_MODE
    // TODO remove later, now left just for reference on testing times
    std::cout << "Game::LoadGame - START LOADING: " << path << std::endl;
    auto t0 = std::chrono::high_resolution_clock::now();
#endif

    if(mReaderSave != nullptr)
        CloseSaveFileForReading();

    // OPEN save file for reading
    mReaderSave = new utilities::BinaryFile(path, utilities::BinaryFile::OPEN_INPUT);

    if(!mReaderSave->IsOpen())
    {
        std::cout << "[ERR] Game::LoadGame - can't open file " << path << std::endl;

        delete mReaderSave;
        mReaderSave = nullptr;
        return false;
    }

    // VERSION
    std::string version;
    mReaderSave->ReadString(version);

    if(version != SAVE_VERSION)
    {
        std::cout << "[ERR] Game::LoadGame - version in file " << path << " is "
                  << version << " (expected " << SAVE_VERSION << ")" << std::endl;
        return false;
    }

    // HEADER with data used for quick load
    mGameMode = static_cast<GameMode>(mReaderSave->ReadUint());
    mLocalFaction = static_cast<PlayerFaction>(mReaderSave->ReadUint());
    mCurrPlanet = static_cast<PlanetId>(mReaderSave->ReadUint());
    mCurrTerritory = mReaderSave->ReadUint();
    const int turn = mReaderSave->ReadInt();
    const std::time_t timestamp = mReaderSave->ReadTimeT();

    // game data
    mDifficulty = static_cast<Difficulty>(mReaderSave->ReadUint());
    mRandSeed = mReaderSave->ReadUint();

    // Tutorial
    mTutMan->Load(*mReaderSave);

    mTutorialEnabled = mReaderSave->ReadBool();

    // Planets
    const unsigned int numPlanets = mReaderSave->ReadUint();

    for(unsigned int i = 0; i < numPlanets; ++i)
    {
        auto p = new Planet;
        p->Load(*mReaderSave);

        mPlanets.emplace(p->GetPlanetId(), p);
    }

    // Players
    const unsigned int numPlayers = mReaderSave->ReadUint();

    for(unsigned int i = 0; i < numPlayers; ++i)
    {
        auto p = new Player;
        p->Load(*mReaderSave);

        mPlayers.emplace_back(p);
    }

    // active Players
    const unsigned int numActivePlayers = mReaderSave->ReadUint();

    for(unsigned int i = 0; i < numActivePlayers; ++i)
    {
        const int playerId = mReaderSave->ReadInt();

        for(auto p : mPlayers)
        {
            if(p->GetPlayerId() == playerId)
                mActivePlayers.emplace_back(p);
        }
    }

    // AI Players
    const unsigned int numAIPlayers = mReaderSave->ReadUint();

    for(unsigned int i = 0; i < numAIPlayers; ++i)
    {
        const int playerId = mReaderSave->ReadInt();
        const bool active = mReaderSave->ReadBool();

        for(auto p : mPlayers)
        {
            if(p->GetPlayerId() == playerId)
            {
                auto * ai = new PlayerAI(p, mObjsRegistry);
                ai->SetActive(active);

                p->SetAI(ai);

                mAIPlayers.emplace_back(p);
            }
        }
    }

    // save cost of saving
    mCostSave[ER_MONEY] = mReaderSave->ReadInt();
    mCostSave[ER_ENERGY] = mReaderSave->ReadInt();
    mCostSave[ER_MATERIAL] = mReaderSave->ReadInt();

    // State
    const auto stateId = static_cast<StateId>(mReaderSave->ReadUint());

    // move to screen for loading
    StateDataLoadGame data(stateId);
    RequestNextActiveState(StateId::LOAD_GAME, &data);

#ifdef DEV_MODE
    // TODO remove later, now left just for reference on testing times
    auto t1 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    std::cout << "Game::LoadGame - GAME LOADED in: " << duration.count() << " ms" << std::endl;
#endif

    if(!saveFile.empty())
        mCurrSaveFile = saveFile;

    return true;
}

bool Game::SaveGame(const std::string & saveFile)
{
    using namespace sgl;

    const std::string path = saveFile.empty() ? (mDirSave + mCurrSaveFile) : (mDirSave + saveFile);

#ifdef DEV_MODE
    // TODO remove later, now left just for reference on testing times
    std::cout << "Game::SaveGame - START SAVING: " << path << std::endl;
    auto t0 = std::chrono::high_resolution_clock::now();
#endif

    // OPEN save file
    utilities::BinaryFile bf(path, utilities::BinaryFile::OPEN_OUTPUT, true);

    if(!bf.IsOpen())
    {
        std::cout << "[ERR] Game::SaveGame - can't open file " << path << std::endl;
        return false;
    }

    // version
    bf.WriteString(SAVE_VERSION);

    // HEADER with data used for quick load
    bf.WriteUint(mGameMode);
    bf.WriteUint(mLocalFaction);
    bf.WriteUint(mCurrPlanet);
    bf.WriteUint(mCurrTerritory);
    bf.WriteInt(GetCurrentLocalTurn());
    const auto now = std::chrono::system_clock::now();
    bf.WriteTimeT(std::chrono::system_clock::to_time_t(now));

    // game data
    bf.WriteUint(mDifficulty);
    bf.WriteUint(mRandSeed);

    // Tutorial
    mTutMan->Save(bf);
    // save if tutorial is enabled in game file as it's a game specific setting
    bf.WriteBool(mTutorialEnabled);

    // Planets
    const unsigned int numPlanets = mPlanets.size();
    bf.WriteUint(numPlanets);

    for(auto it : mPlanets)
        it.second->Save(bf);

    // Players
    const unsigned int numPlayers = mPlayers.size();
    bf.WriteUint(numPlayers);

    for(Player * p : mPlayers)
        p->Save(bf);

    // active Players
    const unsigned int numActivePlayers = mActivePlayers.size();
    bf.WriteUint(numActivePlayers);

    for(Player * p : mActivePlayers)
        bf.WriteInt(p->GetPlayerId());

    // AI Players
    const unsigned int numAIPlayers = mAIPlayers.size();
    bf.WriteUint(numAIPlayers);

    for(Player * p : mAIPlayers)
    {
        auto ai = p->GetAI();

        bf.WriteInt(p->GetPlayerId());
        bf.WriteBool(ai->IsActive());
    }

    // reset cost of saving before writing the values
    InitCostSaveGame();

    // save cost of saving
    bf.WriteInt(mCostSave[ER_MONEY]);
    bf.WriteInt(mCostSave[ER_ENERGY]);
    bf.WriteInt(mCostSave[ER_MATERIAL]);

    // NOTE KEEP THIS LAST SAVE BLOCK
    // State
    const unsigned int stateId = GetActiveStateId();
    bf.WriteUint(stateId);

    auto state = static_cast<BaseGameState *>(mStateMan->GetActiveState());
    state->Save(bf);

    // CLOSE save file
    bf.Close();

#ifdef DEV_MODE
    // TODO remove later, now left just for reference on testing times
    auto t1 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    std::cout << "Game::SaveGame - GAME SAVED in: " << duration.count() << " ms" << std::endl;
#endif

    return true;
}

bool Game::LoadSettings()
{
    using namespace sgl;

    const std::string path = mDirSettings + mSettingsFile;

#ifdef DEV_MODE
    // TODO remove later, now left just for reference on testing times
    std::cout << "Game::LoadSettings - START LOADING: " << path << std::endl;
    auto t0 = std::chrono::high_resolution_clock::now();
#endif

    // OPEN settings file
    utilities::BinaryFile bf(path, utilities::BinaryFile::OPEN_INPUT, false);

    if(!bf.IsOpen())
    {
        std::cout << "[ERR] Game::LoadSettings - can't open file " << path << std::endl;
        return false;
    }

    // version
    std::string ver;
    bf.ReadString(ver);

    if(ver != SETTINGS_VERSION)
    {
        std::cout << "[ERR] Game::LoadSettings - loaded version different from expected: " << ver
                  << " (" << SETTINGS_VERSION << ")"  << std::endl;
        return false;
    }

    // settings
    const unsigned int lang = bf.ReadUint();
    SetLanguage(static_cast<LanguageId>(lang));

    mMapDraggingSpeed = bf.ReadInt();
    mMapScrollingSpeed = bf.ReadInt();
    mButtonSelect = bf.ReadInt();
    mButtonAction = bf.ReadInt();

    const int display = bf.ReadInt();
    const int dMode = bf.ReadInt();
    const auto vMode = static_cast<graphic::Window::VideoMode>(bf.ReadInt());
    const bool grabMouse = bf.ReadBool();
    mTimeAutoHideMouse = bf.ReadFloat();
    SetAutoHideMouseEnabled(bf.ReadBool());

    if(display >= 0 && dMode >= 0)
    {
        if(mWin->SetDisplayMode(display, dMode))
            mWin->SetVideoMode(vMode);
    }

    mWin->GrabMouse(grabMouse);

    mMapDragging = bf.ReadBool();
    mMapScrollingOnEdges = bf.ReadBool();
    mMapScrollingConstSpeed = bf.ReadBool();
    mAutoUnitCamera = bf.ReadBool();
    mAutoSave = bf.ReadBool();

#ifdef DEV_MODE
    // TODO remove later, now left just for reference on testing times
    auto t1 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    std::cout << "Game::LoadSettings - SETTINGS LOADED in: " << duration.count() << " ms" << std::endl;
#endif

    return true;
}

bool Game::SaveSettings()
{
    using namespace sgl;

    const std::string path = mDirSettings + mSettingsFile;

#ifdef DEV_MODE
    // TODO remove later, now left just for reference on testing times
    std::cout << "Game::SaveSettings - START SAVING: " << path << std::endl;
    auto t0 = std::chrono::high_resolution_clock::now();
#endif

    // OPEN settings file
    utilities::BinaryFile bf(path, utilities::BinaryFile::OPEN_OUTPUT, true);

    if(!bf.IsOpen())
    {
        std::cout << "[ERR] Game::SaveSettings - can't open file " << path << std::endl;
        return false;
    }

    // version
    bf.WriteString(SETTINGS_VERSION);

    // settings
    bf.WriteUint(mLanguage);

    bf.WriteInt(mMapDraggingSpeed);
    bf.WriteInt(mMapScrollingSpeed);
    bf.WriteInt(mButtonSelect);
    bf.WriteInt(mButtonAction);

    bf.WriteInt(mWin->GetCurrentDisplayIndex());
    bf.WriteInt(mWin->GetCurrentDisplayModeIndex());
    bf.WriteInt(mWin->GetVideoMode());
    bf.WriteBool(mWin->IsMouseGrabbed());
    bf.WriteFloat(mTimeAutoHideMouse);
    bf.WriteBool(IsAutoHideMouseEnable());

    bf.WriteBool(mMapDragging);
    bf.WriteBool(mMapScrollingOnEdges);
    bf.WriteBool(mMapScrollingConstSpeed);
    bf.WriteBool(mAutoUnitCamera);
    bf.WriteBool(mAutoSave);

#ifdef DEV_MODE
    // TODO remove later, now left just for reference on testing times
    auto t1 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    std::cout << "Game::SaveSettings - SETTINGS SAVED in: " << duration.count() << " ms" << std::endl;
#endif

    return true;
}

#ifdef DEV_MODE
bool Game::SaveGameTurn()
{
    using namespace std;

    ostringstream ss;

    ss << "mission_" << setw(2) << setfill('0') <<  GetCurrentPlanet()->GetPlanetId()
       << "_" << setw(2) << setfill('0') << GetCurrentTerritory()
       << "-" << setw(3) << setfill('0') << GetLocalPlayer()->GetTurnsPlayed() << ".sav";

    return SaveGame(ss.str());
}

bool Game::SaveGamePlanet()
{
using namespace std;

    ostringstream ss;

    ss << "planet_" << setw(2) << setfill('0') <<  GetCurrentPlanet()->GetPlanetId()
       << "_" << setw(2) << setfill('0') << GetCurrentTerritory() << ".sav";

    return SaveGame(ss.str());
}
#endif

void Game::LowerCostSaveGame()
{
    const unsigned int minMoney = 500;
    const unsigned int minEnergy = 100;
    const unsigned int minMaterial = 10;
    const unsigned int decMoney = mCostSave[ER_MONEY] > 1000 ? 1000 : 500;
    const unsigned int decEnergy = 100;
    const unsigned int decMaterial = 10;

    const int newMoney = mCostSave[ER_MONEY] - decMoney;
    mCostSave[ER_MONEY] = (newMoney < minMoney) ? minMoney : newMoney;

    const int newEnergy = mCostSave[ER_ENERGY] - decEnergy;
    mCostSave[ER_ENERGY] = (newEnergy < minEnergy) ? minEnergy : newEnergy;

    const int newMaterial = mCostSave[ER_MATERIAL] - decMaterial;
    mCostSave[ER_MATERIAL] = (newMaterial < minMaterial) ? minMaterial : newMaterial;
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
        20,
        30,
        80,
        90,
        1,
        100,
    };

    static_assert(sizeof(price) / sizeof(int) == NUM_EXT_RESOURCES);

    if(t < NUM_EXT_RESOURCES)
        return price[t];
    else
        return 0;
}

int Game::GetResourcePriceSell(ExtendedResource t) const
{
    // TODO make it change depending on territory/planet
    const int price[] =
    {
        15,
        20,
        70,
        80,
        1,
        90,
    };

    static_assert(sizeof(price) / sizeof(int) == NUM_EXT_RESOURCES);

    if(t < NUM_EXT_RESOURCES)
        return price[t];
    else
        return 0;
}

unsigned int Game::GetActiveStateId() const { return mStateMan->GetActiveStateId(); }

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

int Game::GetCurrentLocalTurn() const
{
    const int noTurn = -1;

    if(NO_FACTION == mLocalFaction)
        return noTurn;

    if(mStateMan->GetActiveStateId() != GAME)
        return noTurn;
    else
        return GetLocalPlayer()->GetTurnsPlayed();
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

void Game::InitCostSaveGame()
{
    const unsigned int maxMoney = 4000;
    const unsigned int maxEnergy = 500;
    const unsigned int maxMaterial = 50;

    mCostSave.insert_or_assign(ER_MONEY, maxMoney);
    mCostSave.insert_or_assign(ER_ENERGY, maxEnergy);
    mCostSave.insert_or_assign(ER_MATERIAL, maxMaterial);
}

void Game::CreatePlayers()
{
    assert(mLocalFaction != NO_FACTION);

    // create a Player for each faction
    for(unsigned int i = 0; i < NUM_FACTIONS; ++i)
    {
        auto f = static_cast<PlayerFaction>(i);

        auto p = new Player(i);
        p->SetFaction(f);

        // set initial resources
        p->SetResource(ER_ENERGY, START_ENERGY);
        p->SetResource(ER_MATERIAL, START_MATERIAL);
        p->SetResource(ER_DIAMONDS, START_DIAMONDS);
        p->SetResource(ER_BLOBS, START_BLOBS);
        p->SetResource(ER_MONEY, START_MONEY);
        p->SetResource(ER_RESEARCH, START_RESEARCH);

        if(f == mLocalFaction)
        {
            InitPlayerLocal(p);

            // add local player as first active one
            mActivePlayers.emplace_back(p);
        }
        else
        {
            InitPlayerAI(p);

            mAIPlayers.emplace_back(p);
        }

        mPlayers.emplace_back(p);
    }
}

void Game::InitPlayerLocal(Player * p)
{
    p->SetMaxUnits(MAX_UNITS0);

    // assign initial available structures
    p->AddAvailableStructure(ObjectData::TYPE_BARRACKS);
    p->AddAvailableStructure(ObjectData::TYPE_BUNKER);
    p->AddAvailableStructure(ObjectData::TYPE_DEFENSIVE_TOWER);
    p->AddAvailableStructure(ObjectData::TYPE_HOSPITAL);
    p->AddAvailableStructure(ObjectData::TYPE_RESEARCH_CENTER);
    p->AddAvailableStructure(ObjectData::TYPE_RES_GEN_ENERGY_SOLAR);
    p->AddAvailableStructure(ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT);
    p->AddAvailableStructure(ObjectData::TYPE_WALL_GATE);

    // assign initial available units
    p->AddAvailableUnit(ObjectData::TYPE_UNIT_WORKER1);
    p->AddAvailableUnit(ObjectData::TYPE_UNIT_SOLDIER1);
    p->AddAvailableUnit(ObjectData::TYPE_UNIT_SPAWNER1);
    p->AddAvailableUnit(ObjectData::TYPE_UNIT_SCOUT1);
    p->AddAvailableUnit(ObjectData::TYPE_UNIT_SOLDIER2);
    p->AddAvailableUnit(ObjectData::TYPE_UNIT_MEDIC1);

    // assign initial available mini units
    p->AddAvailableMiniUnit(ObjectData::TYPE_MINI_UNIT1);
    p->AddAvailableMiniUnit(ObjectData::TYPE_MINI_UNIT2);
}

void Game::InitPlayerAI(Player * p)
{
    p->SetMaxUnits(MAX_UNITS0);

    auto * ai = new PlayerAI(p, mObjsRegistry);
    p->SetAI(ai);

    // assign initial available structures
    p->AddAvailableStructure(ObjectData::TYPE_BARRACKS);
    p->AddAvailableStructure(ObjectData::TYPE_BUNKER);
    p->AddAvailableStructure(ObjectData::TYPE_DEFENSIVE_TOWER);
    p->AddAvailableStructure(ObjectData::TYPE_HOSPITAL);
    p->AddAvailableStructure(ObjectData::TYPE_PRACTICE_TARGET);
    p->AddAvailableStructure(ObjectData::TYPE_RADAR_STATION);
    p->AddAvailableStructure(ObjectData::TYPE_RADAR_TOWER);
    p->AddAvailableStructure(ObjectData::TYPE_RESEARCH_CENTER);
    p->AddAvailableStructure(ObjectData::TYPE_RES_GEN_ENERGY_SOLAR);
    p->AddAvailableStructure(ObjectData::TYPE_RES_GEN_MATERIAL_EXTRACT);
    p->AddAvailableStructure(ObjectData::TYPE_RES_STORAGE_BLOBS);
    p->AddAvailableStructure(ObjectData::TYPE_RES_STORAGE_DIAMONDS);
    p->AddAvailableStructure(ObjectData::TYPE_RES_STORAGE_ENERGY);
    p->AddAvailableStructure(ObjectData::TYPE_RES_STORAGE_MATERIAL);
    p->AddAvailableStructure(ObjectData::TYPE_TRADING_POST);
    p->AddAvailableStructure(ObjectData::TYPE_WALL_GATE);

    // assign initial available units
    p->AddAvailableUnit(ObjectData::TYPE_UNIT_WORKER1);
    p->AddAvailableUnit(ObjectData::TYPE_UNIT_SOLDIER1);
    p->AddAvailableUnit(ObjectData::TYPE_UNIT_SCOUT1);
    p->AddAvailableUnit(ObjectData::TYPE_UNIT_SOLDIER2);
    p->AddAvailableUnit(ObjectData::TYPE_UNIT_MEDIC1);
}

void Game::CreatePlanets()
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
    planet->AddMap("data/maps/01-e1.map", NO_FACTION, TER_ST_UNEXPLORED);
    planet->AddMap("data/maps/01-02.map", NO_FACTION, TER_ST_UNREACHABLE);
    planet->AddMap("data/maps/01-e2.map", NO_FACTION, TER_ST_UNREACHABLE);
    planet->AddMap("data/maps/01-03.map", NO_FACTION, TER_ST_UNREACHABLE);

    mPlanets.emplace(PLANET_1, planet);
}

void Game::ClearPlayers()
{
    for(Player * p : mPlayers)
        delete p;

    mPlayers.clear();
    mActivePlayers.clear();
    mAIPlayers.clear();

    mLocalFaction = NO_FACTION;
}

void Game::ClearPlanets()
{
    for(auto it : mPlanets)
        delete it.second;

    mPlanets.clear();
}

Player * Game::GetPlayerByFaction(PlayerFaction faction) const
{
    if(faction < NUM_FACTIONS)
        return mPlayers[faction];
    else
        return nullptr;
}

Player * Game::GetActivePlayerByFaction(PlayerFaction faction) const
{
    for(Player * p : mActivePlayers)
    {
        if(p->GetFaction() == faction)
            return p;
    }

    return nullptr;
}

void Game::ClearAllAIActivePlayers()
{
    mActivePlayers.erase(mActivePlayers.begin() + 1, mActivePlayers.end());
}

void Game::AddToActivePlayersRandomAI()
{
    assert(!mAIPlayers.empty());

    const unsigned int lastAI = mAIPlayers.size() - 1;

    sgl::utilities::UniformDistribution ud(0, lastAI);

    auto p = mAIPlayers[ud.GetNextValue()];

    mActivePlayers.emplace_back(p);
}

void Game::AddToActivePlayersAI(PlayerFaction f)
{
    assert(!mAIPlayers.empty());

    for(Player * p : mAIPlayers)
    {
        if(p->GetFaction() == f)
        {
            mActivePlayers.emplace_back(p);
            return ;
        }
    }
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

bool Game::IsAutoHideMouseEnable() const
{
    return sgl::sgui::Stage::Instance()->IsAutoHidingCursor();
}

void Game::SetAutoHideMouseEnabled(bool val)
{
    sgl::sgui::Stage::Instance()->AutoHideInactiveCursor(val, mTimeAutoHideMouse);
}

} // namespace game
