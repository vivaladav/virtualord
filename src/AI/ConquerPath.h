#pragma once

#include <vector>

namespace game
{

class ConquestIndicator;
class GameMap;
class GameMapProgressBar;
class IsoMap;
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
    ConquerPath(Unit * unit, IsoMap * im, GameMap * gm, ScreenGame * sg);
    ~ConquerPath();

    Unit * GetUnit() const;

    ConquerState GetState() const;

    float GetPathCost() const;

    void SetPathCells(const std::vector<unsigned int> & cells);

    bool HasStarted() const;
    bool Start();
    void Abort();
    void InstantAbort();

    void Update(float delta);

private:
    void CreateIndicators();

    bool InitNextConquest();
    bool InitNextMove();

    void UpdateMove(float delta);

    void UpdatePathCost();

    bool Fail();
    bool Finish();

private:
    std::vector<unsigned int> mCells;

    std::vector<ConquestIndicator *> mIndicators;

    GameMapProgressBar * mProgressBar = nullptr;

    Unit * mUnit = nullptr;

    IsoMap * mIsoMap = nullptr;
    GameMap * mGameMap = nullptr;

    ScreenGame * mScreen = nullptr;

    ConquerState mState = READY;

    unsigned int mNextCell = 0;

    float mCost = 0.f;

    // movement
    float mObjX = 0.f;
    float mObjY = 0.f;
    float mVelX = 0.f;
    float mVelY = 0.f;
    float mTargetX = 0.f;
    float mTargetY = 0.f;

    bool mLocalPlayer = false;
};

inline Unit * ConquerPath::GetUnit() const { return mUnit; }

inline ConquerPath::ConquerState ConquerPath::GetState() const { return mState; }

inline float ConquerPath::GetPathCost() const { return mCost; }

inline bool ConquerPath::HasStarted() const { return mState != READY; }

inline void ConquerPath::SetPathCells(const std::vector<unsigned int> & cells)
{
    mCells = cells;

    UpdatePathCost();
}

} // namespace game
