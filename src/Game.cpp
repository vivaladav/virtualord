#include "Game.h"

#include "GameConstants.h"
#include "Planet.h"
#include "Player.h"
#include "ResourceLoader.h"
#include "Version.h"
#include "AI/PlayerAI.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "States/StatesIds.h"
#include "States/StateFactionSelection.h"
#include "States/StateGame.h"
#include "States/StateInit.h"
#include "States/StateInitGame.h"
#include "States/StateLeaveGame.h"
#include "States/StateLeavePregame.h"
#include "States/StateLoadGame.h"
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
#include <sgl/utilities/UniformDistribution.h>

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

const std::string Game::SAVE_VERSION("0.1.0");
const std::string Game::SETTINGS_VERSION("0.1.0");

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
    mStateMan->AddState(new StateLoadGame(this));
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
    mCostUpgrades.emplace(TECH_UP_BASE_IMPROVE_4, 7000);
    mCostUpgrades.emplace(TECH_UP_BASE_IMPROVE_5, 11000);
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
    mCostUpgrades.emplace(TECH_UP_UNIT_SLOTS_1, 2000);
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

    mSettingsFile = mDirSave + std::string("settings.sav");

    // load settings when starting
    LoadSettings();
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

bool Game::IsSaveFileValid() const
{
    using namespace sgl;

    // OPEN save file
    utilities::BinaryFile bf(mCurrSaveFile, utilities::BinaryFile::OPEN_INPUT);

    if(!bf.IsOpen())
    {
        std::cout << "[ERR] Game::IsSaveFileValid - can't open file " << mCurrSaveFile << std::endl;
        return false;
    }

    // version
    std::string version;
    bf.ReadString(version);

    if(version != SAVE_VERSION)
    {
        std::cout << "[ERR] Game::IsSaveFileValid - version in file " << mCurrSaveFile << " is "
                  << version << " (expected " << SAVE_VERSION << ")" << std::endl;
        return false;
    }

    // CLOSE save file
    bf.Close();

    return true;
}

bool Game::LoadGame()
{
    using namespace sgl;

#ifdef DEV_MODE
    // TODO remove later, now left just for reference on testing times
    std::cout << "Game::LoadGame - START LOADING: " << mCurrSaveFile << std::endl;
    auto t0 = std::chrono::high_resolution_clock::now();
#endif

    if(mReaderSave != nullptr)
        CloseSaveFileForReading();

    // OPEN save file for reading
    mReaderSave = new utilities::BinaryFile(mCurrSaveFile, utilities::BinaryFile::OPEN_INPUT);

    if(!mReaderSave->IsOpen())
    {
        std::cout << "[ERR] Game::GetSaveFileForReading - can't open file " << mCurrSaveFile << std::endl;

        delete mReaderSave;
        mReaderSave = nullptr;
    }

    // version
    std::string version;
    mReaderSave->ReadString(version);

    if(version != SAVE_VERSION)
    {
        std::cout << "[ERR] Game::IsSaveFileValid - version in file " << mCurrSaveFile << " is "
                  << version << " (expected " << SAVE_VERSION << ")" << std::endl;
        return false;
    }

    // game data
    mDifficulty = static_cast<Difficulty>(mReaderSave->ReadUint());
    mLocalFaction = static_cast<PlayerFaction>(mReaderSave->ReadUint());
    mCurrPlanet = static_cast<PlanetId>(mReaderSave->ReadUint());
    mCurrTerritory = mReaderSave->ReadUint();
    mRandSeed = mReaderSave->ReadUint();

    // Tutorial
    mTutMan->Load(*mReaderSave);

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

    return true;
}

bool Game::SaveGame()
{
    using namespace sgl;

#ifdef DEV_MODE
    // TODO remove later, now left just for reference on testing times
    std::cout << "Game::SaveGame - START SAVING: " << mCurrSaveFile << std::endl;
    auto t0 = std::chrono::high_resolution_clock::now();
#endif

    // OPEN save file
    utilities::BinaryFile bf(mCurrSaveFile, utilities::BinaryFile::OPEN_OUTPUT, true);

    if(!bf.IsOpen())
    {
        std::cout << "[ERR] Game::SaveGame - can't open file " << mCurrSaveFile << std::endl;
        return false;
    }

    // version
    bf.WriteString(SAVE_VERSION);

    // game data
    bf.WriteUint(mDifficulty);
    bf.WriteUint(mLocalFaction);
    bf.WriteUint(mCurrPlanet);
    bf.WriteUint(mCurrTerritory);
    bf.WriteUint(mRandSeed);

    // Tutorial
    mTutMan->Save(bf);

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

#ifdef DEV_MODE
    // TODO remove later, now left just for reference on testing times
    std::cout << "Game::LoadSettings - START LOADING: " << mSettingsFile << std::endl;
    auto t0 = std::chrono::high_resolution_clock::now();
#endif

    // OPEN settings file
    utilities::BinaryFile bf(mSettingsFile, utilities::BinaryFile::OPEN_INPUT, false);

    if(!bf.IsOpen())
    {
        std::cout << "[ERR] Game::LoadSettings - can't open file " << mSettingsFile << std::endl;
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

    mTimeAutoHideMouse = bf.ReadFloat();

    mMapDragging = bf.ReadBool();
    mMapScrollingOnEdges = bf.ReadBool();
    mMapScrollingConstSpeed = bf.ReadBool();
    mAutoEndTurn = bf.ReadBool();
    mAutoUnitCamera = bf.ReadBool();
    mTutorialEnabled = bf.ReadBool();
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

#ifdef DEV_MODE
    // TODO remove later, now left just for reference on testing times
    std::cout << "Game::SaveSettings - START SAVING: " << mSettingsFile << std::endl;
    auto t0 = std::chrono::high_resolution_clock::now();
#endif

    // OPEN settings file
    utilities::BinaryFile bf(mSettingsFile, utilities::BinaryFile::OPEN_OUTPUT, true);

    if(!bf.IsOpen())
    {
        std::cout << "[ERR] Game::SaveSettings - can't open file " << mSettingsFile << std::endl;
        return false;
    }

    // version
    bf.WriteString(SAVE_VERSION);

    // settings
    bf.WriteUint(mLanguage);

    bf.WriteInt(mMapDraggingSpeed);
    bf.WriteInt(mMapScrollingSpeed);
    bf.WriteInt(mButtonSelect);
    bf.WriteInt(mButtonAction);

    bf.WriteFloat(mTimeAutoHideMouse);

    bf.WriteBool(mMapDragging);
    bf.WriteBool(mMapScrollingOnEdges);
    bf.WriteBool(mMapScrollingConstSpeed);
    bf.WriteBool(mAutoEndTurn);
    bf.WriteBool(mAutoUnitCamera);
    bf.WriteBool(mTutorialEnabled);
    bf.WriteBool(mAutoSave);

#ifdef DEV_MODE
    // TODO remove later, now left just for reference on testing times
    auto t1 = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0);
    std::cout << "Game::SaveSettings - SETTINGS SAVED in: " << duration.count() << " ms" << std::endl;
#endif

    return true;
}

void Game::LowerCostSaveGame()
{
    const unsigned int minMoney = 1000;
    const unsigned int minEnergy = 100;
    const unsigned int minMaterial = 10;
    const unsigned int decMoney = 1000;
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
        15,
        20,
        70,
        80,
        1,
        90,
    };

    static_assert(sizeof(price) / sizeof(int) == NUM_EXTENDED_RESOURCES);

    if(t < NUM_EXTENDED_RESOURCES)
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
    const unsigned int maxMoney = 10000;
    const unsigned int maxEnergy = 1000;
    const unsigned int maxMaterial = 100;

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
        p->SetResource(Player::Stat::BLOBS, START_BLOBS);
        p->SetResource(Player::Stat::DIAMONDS, START_DIAMONDS);
        p->SetResource(Player::Stat::ENERGY, START_ENERGY);
        p->SetResource(Player::Stat::MATERIAL, START_MATERIAL);
        p->SetResource(Player::Stat::MONEY, START_MONEY);
        p->SetResource(Player::Stat::RESEARCH, START_RESEARCH);

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
    p->AddAvailableStructure(ObjectData::TYPE_SPAWN_TOWER);
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
    planet->AddMap("data/maps/60x60-01.map", NO_FACTION, TER_ST_UNEXPLORED);
    planet->AddMap("data/maps/01-02.map", NO_FACTION, TER_ST_UNREACHABLE);
    planet->AddMap("data/maps/80x80-01.map", NO_FACTION, TER_ST_UNREACHABLE);
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

} // namespace game
