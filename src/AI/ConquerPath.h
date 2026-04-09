#pragma once

#include <vector>

namespace game
{

class GameMap;
class GameMapProgressBar;
class OverlayCellConquest;
class Player;
class ScreenGame;
class Unit;

class ConquerPath
{
public:
    enum ConquerState : unsigned int
    {
        READY,
        MOVING,
        CONQUERING,
        COMPLETED,
        FAILED,
        ABORTING,
        ABORTED,

        NUM_PATH_STATES
    };

public:
    ConquerPath(Unit * unit, GameMap * gm, ScreenGame * sg, OverlayCellConquest * overlay = nullptr);
    ~ConquerPath();

    Unit * GetUnit() const;

    ConquerState GetState() const;

    int GetCostUnitEnergy() const;
    int GetCostResourceEnergy() const;
    int GetCostResourceMaterial() const;

    void SetPathCells(const std::vector<unsigned int> & cells);

    bool HasStarted() const;
    bool Start();
    void Abort();
    void InstantAbort();

    void Update(float delta);

private:
    bool HasResourcesToConquerCell();

    bool InitNextConquest();
    bool InitNextMove();

    void UpdateMove(float delta);

    void UpdatePathCost();

    bool Fail();
    bool Finish();

private:
    std::vector<unsigned int> mCells;

    OverlayCellConquest * mOverlay = nullptr;

    GameMapProgressBar * mProgressBar = nullptr;

    Unit * mUnit = nullptr;
    Player * mPlayer = nullptr;

    GameMap * mGameMap = nullptr;

    ScreenGame * mScreen = nullptr;

    ConquerState mState = READY;

    unsigned int mNextCell = 0;

    int mCostUnitEnergy = 0;
    int mCostResEnergy = 0;
    int mCostResMaterial = 0;

    // movement
    float mObjX = 0.f;
    float mObjY = 0.f;
    float mVelX = 0.f;
    float mVelY = 0.f;
    float mTargetX = 0.f;
    float mTargetY = 0.f;
};

inline Unit * ConquerPath::GetUnit() const { return mUnit; }

inline ConquerPath::ConquerState ConquerPath::GetState() const { return mState; }

inline int ConquerPath::GetCostUnitEnergy() const { return mCostUnitEnergy; }
inline int ConquerPath::GetCostResourceEnergy() const { return mCostResEnergy; }
inline int ConquerPath::GetCostResourceMaterial() const { return mCostResMaterial; }

inline bool ConquerPath::HasStarted() const { return mState != READY; }

inline void ConquerPath::SetPathCells(const std::vector<unsigned int> & cells)
{
    mCells = cells;

    UpdatePathCost();
}

} // namespace game
