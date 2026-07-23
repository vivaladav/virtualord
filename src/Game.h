#pragma once

#include <sgl/core/Application.h>

#include <functional>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>

namespace sgl
{
    namespace graphic
    {
        class Cursor;
        class Font;
        class Renderer;
        class Window;
    }

    namespace media { class AudioManager; }

    namespace sgui { class Stage; }

    namespace utilities
    {
        class StateData;
        class StateManager;
    }
}

namespace game
{

class ObjectsDataRegistry;
class Planet;
class Player;
class TutorialManager;

enum ExtendedResource : unsigned int;
enum GameCursorId : unsigned int;
enum LanguageId : unsigned int;
enum PlanetId : unsigned int;
enum PlayerFaction : unsigned int;
enum StateId : unsigned int;
enum TechUpgradeId : unsigned int;

enum Difficulty : unsigned int
{
    EASY,
    MEDIUM,
    HARD,

    NUM_DIFFICULTIES
};


class Game : public sgl::core::Application
{
#ifdef DEV_MODE
public:
    static bool GOD_MODE;
#endif

public:
    static const std::string SAVE_VERSION;
    static const std::string SETTINGS_VERSION;

public:
    Game(int argc, char * argv[]);
    ~Game();

    void InitNewGameData();
    void ClearGameData();

    // -- LOAD & SAVE --
    const std::string & GetCurrentSaveFile() const;
    bool LoadGame();
    bool SaveGame();
    bool LoadSettings();
    bool SaveSettings();

    // -- mouse cursors --
    void RegisterCursor(GameCursorId curId, sgl::graphic::Cursor * cursor);
    void SetCurrentCursor(GameCursorId curId);

    // -- planets --
    Planet * GetPlanet(PlanetId pid) const;
    const std::string & GetCurrentMapFile() const;
    unsigned int GetCurrentTerritory() const;
    void SetCurrentTerritory(unsigned int territory);
    Planet * GetCurrentPlanet() const;
    void SetCurrentPlanet(PlanetId planet);

    int GetResourcePriceBuy(ExtendedResource t) const;
    int GetResourcePriceSell(ExtendedResource t) const;

    void SetClearColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

    // GAME STATE
    unsigned int GetActiveStateId() const;
    void RequestNextActiveState(StateId sid, sgl::utilities::StateData * data = nullptr);

    Difficulty GetDifficulty() const;
    void SetDifficulty(Difficulty level);

    const ObjectsDataRegistry * GetObjectsRegistry() const;

    // -- players --
    int GetNumActivePlayers() const;

    Player * GetActivePlayerByIndex(unsigned int index) const;
    Player * GetActivePlayerByFaction(PlayerFaction faction) const;
    void ClearAllAIActivePlayers();
    void AddToActivePlayersRandomAI();
    void AddToActivePlayersAI(PlayerFaction f);

    Player * GetLocalPlayer() const;

    void SetLocalPlayerFaction(PlayerFaction faction);
    PlayerFaction GetLocalPlayerFaction() const;

    // -- tech upgrades --
    int GetTechUpgradecost(TechUpgradeId upgrade) const;

    // -- settings --
    LanguageId GetLanguage() const;
    void SetLanguage(LanguageId lang);

    int GetMapDraggingSpeed() const;
    void SetMapDraggingSpeed(int val);
    bool IsMapDragging() const;
    void SetMapDragging(bool val);

    int GetMapScrollingSpeed() const;
    void SetMapScrollingSpeed(int val);
    bool IsMapScrollingOnEdges() const;
    void SetMapScrollingOnEdges(bool val);
    bool IsMapScrollingConstant() const;
    void SetMapScrollingConstant(bool val);
    bool IsAutoEndTurnEnabled() const;
    void SetAutoEndTurn(bool val);
    bool IsAutoUnitCameraEnabled() const;
    void SetAutoUnitCamera(bool val);
    bool IsTutorialEnabled() const;
    void SetTutorialEnabled(bool val);

    float GetTimeAutoHideMouse() const;

    int GetButtonSelect() const;
    void SetButtonSelect(int btn);
    int GetButtonAction() const;
    void SetButtonAction(int btn);

    unsigned int AddOnSettingsChangedFunction(const std::function<void()> & f);
    void RemoveOnSettingsChangedFunction(unsigned int fId);

    void SetRandSeed(unsigned int seed);
    unsigned int GetRandSeed() const;

    TutorialManager * GetTutorialManager() const;

private:
    void CreatePlayers();
    void InitPlayerLocal(Player * p);
    void InitPlayerAI(Player * p);

    void CreatePlanets();

    void ClearPlayers();
    void ClearPlanets();

    void NotifyOnSettingsChanged();

    void Update(float delta) override;

    // -- LOAD & SAVE --
    void InitDirectories();

private:
    std::vector<Player *> mPlayers;
    std::vector<Player *> mActivePlayers;
    std::vector<Player *> mAIPlayers;

    std::unordered_map<PlanetId, Planet *> mPlanets;

    std::unordered_map<TechUpgradeId, int> mCostUpgrades;

    std::map<unsigned int, std::function<void()>> mOnSettingsChanged;

    std::unordered_map<GameCursorId, sgl::graphic::Cursor *> mCursors;

    std::string mDirSave;
    std::string mCurrSaveFile;
    std::string mSettingsFile;

    sgl::graphic::Renderer * mRenderer = nullptr;
    sgl::graphic::Window * mWin = nullptr;

    sgl::utilities::StateManager * mStateMan = nullptr;

    sgl::sgui::Stage * mStage = nullptr;

    sgl::media::AudioManager * mAudioMan = nullptr;

    TutorialManager * mTutMan = nullptr;

    ObjectsDataRegistry * mObjsRegistry = nullptr;

    Difficulty mDifficulty = EASY;

    PlayerFaction mLocalFaction;

    PlanetId mCurrPlanet;
    unsigned int mCurrTerritory = 0;

    unsigned int mRandSeed = 0;

    // SETTINGS
    LanguageId mLanguage;
    int mMapDraggingSpeed = 5;
    int mMapScrollingSpeed = 5;
    int mButtonSelect;
    int mButtonAction;
    float mTimeAutoHideMouse = 2.f;
    bool mMapDragging = true;
    bool mMapScrollingOnEdges = true;
    bool mMapScrollingConstSpeed = false;
    bool mAutoEndTurn = true;
    bool mAutoUnitCamera = true;
    bool mTutorialEnabled = true;

    unsigned char mClearR = 0;
    unsigned char mClearG = 0;
    unsigned char mClearB = 0;
    unsigned char mClearA = 255;
};

inline const std::string & Game::GetCurrentSaveFile() const { return mCurrSaveFile; }

inline unsigned int Game::GetCurrentTerritory() const { return mCurrTerritory; }
inline void Game::SetCurrentTerritory(unsigned int territory)
{
    mCurrTerritory = territory;
}

inline void Game::SetCurrentPlanet(PlanetId planet) { mCurrPlanet = planet; }

inline void Game::SetClearColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    mClearR = r;
    mClearG = g;
    mClearB = b;
    mClearA = a;
}

inline Difficulty Game::GetDifficulty() const { return mDifficulty; }
inline void Game::SetDifficulty(Difficulty level) { mDifficulty = level; }

inline const ObjectsDataRegistry * Game::GetObjectsRegistry() const { return mObjsRegistry; }

inline int Game::GetNumActivePlayers() const { return mActivePlayers.size(); }

inline Player * Game::GetActivePlayerByIndex(unsigned int index) const
{
    if(index < mActivePlayers.size())
        return mActivePlayers[index];
    else
        return nullptr;
}

inline Player * Game::GetLocalPlayer() const
{
    // NOTE for now local player is always at index 0. This might change in the future
    const unsigned int indLocal = 0;
    return mActivePlayers[indLocal];
}

inline void Game::SetLocalPlayerFaction(PlayerFaction faction) { mLocalFaction = faction; }
inline PlayerFaction Game::GetLocalPlayerFaction() const { return mLocalFaction; }

inline int Game::GetTechUpgradecost(TechUpgradeId upgrade) const
{
    auto it = mCostUpgrades.find(upgrade);

    if(it != mCostUpgrades.end())
        return it->second;
    else
        return 0;
}

inline LanguageId Game::GetLanguage() const { return mLanguage; }

inline int Game::GetMapDraggingSpeed() const { return mMapDraggingSpeed; }
inline bool Game::IsMapDragging() const { return mMapDragging; }
inline void Game::SetMapDragging(bool val)
{
    if(val != mMapDragging)
    {
        mMapDragging = val;
        NotifyOnSettingsChanged();
    }
}

inline int Game::GetMapScrollingSpeed() const { return mMapScrollingSpeed; }
inline bool Game::IsMapScrollingOnEdges() const { return mMapScrollingOnEdges; }
inline void Game::SetMapScrollingOnEdges(bool val)
{
    if(val != mMapScrollingOnEdges)
    {
        mMapScrollingOnEdges = val;
        NotifyOnSettingsChanged();
    }
}
inline bool Game::IsMapScrollingConstant() const { return mMapScrollingConstSpeed; }
inline void Game::SetMapScrollingConstant(bool val) { mMapScrollingConstSpeed = val; }
inline bool Game::IsAutoEndTurnEnabled() const { return mAutoEndTurn; }
inline void Game::SetAutoEndTurn(bool val)
{
    if(val != mAutoEndTurn)
    {
        mAutoEndTurn = val;
        NotifyOnSettingsChanged();
    }
}

inline bool Game::IsAutoUnitCameraEnabled() const { return mAutoUnitCamera; }
inline void Game::SetAutoUnitCamera(bool val) { mAutoUnitCamera = val; }

inline bool Game::IsTutorialEnabled() const { return mTutorialEnabled; }
inline void Game::SetTutorialEnabled(bool val) { mTutorialEnabled = val; }

inline float Game::GetTimeAutoHideMouse() const { return mTimeAutoHideMouse; }

inline int Game::GetButtonSelect() const { return mButtonSelect; }
inline void Game::SetButtonSelect(int btn) { mButtonSelect = btn; }
inline int Game::GetButtonAction() const { return mButtonAction; }
inline void Game::SetButtonAction(int btn) { mButtonAction = btn; }

inline void Game::SetRandSeed(unsigned int seed) { mRandSeed = seed; }
inline unsigned int Game::GetRandSeed() const { return mRandSeed; }

inline TutorialManager * Game::GetTutorialManager() const { return mTutMan; }

} // namespace game
