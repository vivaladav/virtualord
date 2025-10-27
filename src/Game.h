#pragma once

#include <sgl/core/Application.h>

#include <functional>
#include <map>
#include <string>
#include <vector>

namespace sgl
{
    namespace graphic
    {
        class Font;
        class Renderer;
        class Window;
    }

    namespace media { class AudioManager; }

    namespace sgui { class Stage; }

    namespace utilities { class StateManager; }
}

namespace game
{

class MapsRegistry;
class ObjectsDataRegistry;
class Player;

enum Planets : unsigned int;
enum PlayerFaction : unsigned int;
enum ResourceType : unsigned int;
enum StateId : int;
enum TutorialId : unsigned int;
enum TutorialState : unsigned int;

enum Difficulty : unsigned int
{
    EASY,
    MEDIUM,
    HARD,

    NUM_DIFFICULTIES
};

class Game : public sgl::core::Application
{
public:
    Game(int argc, char * argv[]);
    ~Game();

    void InitGameData();
    void ClearGameData();

    const std::string & GetCurrentMapFile() const;
    unsigned int GetCurrentTerritory() const;
    void SetCurrentTerritory(unsigned int territory);
    Planets GetCurrentPlanet() const;
    void SetCurrentPlanet(Planets planet);

    int GetResourcePriceBuy(ResourceType t) const;
    int GetResourcePriceSell(ResourceType t) const;

    void SetClearColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);

    void RequestNextActiveState(StateId sid);

    Difficulty GetDifficulty() const;
    void SetDifficulty(Difficulty level);

    MapsRegistry * GetMapsRegistry() const;
    const ObjectsDataRegistry * GetObjectsRegistry() const;

    // -- players --
    Player * AddPlayer(const char * name, int pid);
    void ClearPlayers();

    int GetNumPlayers() const;

    Player * GetPlayerByIndex(unsigned int index) const;
    Player * GetPlayerByFaction(PlayerFaction faction) const;
    Player * GetLocalPlayer() const;

    void SetLocalPlayerFaction(PlayerFaction faction);
    PlayerFaction GetLocalPlayerFaction() const;

    // -- settings --
    int GetMapDraggingSpeed() const;
    void SetMapDraggingSpeed(int val);
    bool IsMapDragging() const;
    void SetMapDragging(bool val);

    int GetMapScrollingSpeed() const;
    void SetMapScrollingSpeed(int val);
    bool IsMapScrollingOnEdges() const;
    void SetMapScrollingOnEdges(bool val);
    bool IsAutoEndTurnEnabled() const;
    void SetAutoEndTurn(bool val);
    bool IsTutorialEnabled() const;
    void SetTutorialEnabled(bool val);

    unsigned int AddOnSettingsChangedFunction(const std::function<void()> & f);
    void RemoveOnSettingsChangedFunction(unsigned int fId);

    // -- tutorial --
    TutorialState GetTutorialState(TutorialId tut);
    void SetTutorialState(TutorialId tut, TutorialState state);

private:
    void NotifyOnSettingsChanged();

    void Update(float delta) override;

private:
    std::vector<Player *> mPlayers;

    std::map<unsigned int, std::function<void()>> mOnSettingsChanged;

    std::vector<TutorialState> mTutorialsState;

    sgl::graphic::Renderer * mRenderer = nullptr;
    sgl::graphic::Window * mWin = nullptr;

    sgl::utilities::StateManager * mStateMan = nullptr;

    sgl::sgui::Stage * mStage = nullptr;

    sgl::media::AudioManager * mAudioMan = nullptr;

    MapsRegistry * mMapsReg = nullptr;
    ObjectsDataRegistry * mObjsRegistry = nullptr;

    Difficulty mDiff = EASY;

    PlayerFaction mLocalFaction;

    unsigned int mCurrMap = 0;
    Planets mCurrPlanet;
    unsigned int mCurrTerritory = 0;

    // SETTINGS
    int mMapDraggingSpeed = 5;
    int mMapScrollingSpeed = 6;
    int mMapDragging = true;
    bool mMapScrollingOnEdges = true;
    bool mAutoEndTurn = true;
    bool mTutorialEnabled = true;

    unsigned char mClearR = 0;
    unsigned char mClearG = 0;
    unsigned char mClearB = 0;
    unsigned char mClearA = 255;
};

inline unsigned int Game::GetCurrentTerritory() const { return mCurrTerritory; }
inline void Game::SetCurrentTerritory(unsigned int territory)
{
    mCurrTerritory = territory;
}

inline Planets Game::GetCurrentPlanet() const { return mCurrPlanet; }
inline void Game::SetCurrentPlanet(Planets planet) { mCurrPlanet = planet; }

inline void Game::SetClearColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
    mClearR = r;
    mClearG = g;
    mClearB = b;
    mClearA = a;
}

inline Difficulty Game::GetDifficulty() const { return mDiff; }
inline void Game::SetDifficulty(Difficulty level) { mDiff = level; }

inline MapsRegistry * Game::GetMapsRegistry() const { return mMapsReg; }
inline const ObjectsDataRegistry * Game::GetObjectsRegistry() const { return mObjsRegistry; }

inline int Game::GetNumPlayers() const { return mPlayers.size(); }

inline Player * Game::GetPlayerByIndex(unsigned int index) const
{
    if(index < mPlayers.size())
        return mPlayers[index];
    else
        return nullptr;
}

inline Player * Game::GetLocalPlayer() const
{
    // NOTE for now local player is always at index 0. This might change in the future
    const int indLocal = 0;
    return mPlayers[indLocal];
}

inline void Game::SetLocalPlayerFaction(PlayerFaction faction)
{
    mLocalFaction = faction;
}

inline PlayerFaction Game::GetLocalPlayerFaction() const
{
    return mLocalFaction;
}

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
inline bool Game::IsAutoEndTurnEnabled() const { return mAutoEndTurn; }
inline void Game::SetAutoEndTurn(bool val)
{
    if(val != mAutoEndTurn)
    {
        mAutoEndTurn = val;
        NotifyOnSettingsChanged();
    }
}

inline bool Game::IsTutorialEnabled() const { return mTutorialEnabled; }
inline void Game::SetTutorialEnabled(bool val) { mTutorialEnabled = val; }

} // namespace game
