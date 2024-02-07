#pragma once

#include <vector>

namespace game
{

class GameMap;
class GameObject;
class IsoMap;
class ScreenGame;

class ObjectPath
{
public:
    enum PathState : unsigned int
    {
        READY,
        MOVING,
        COMPLETED,
        FAILED,
        ABORTING,
        ABORTED,

        NUM_PATH_STATES
    };

public:
    ObjectPath(GameObject * obj, IsoMap * im, GameMap * gm, ScreenGame * sg);

    GameObject * GetObject() const;

    PathState GetState() const;

    float GetPathCost() const;

    void SetPathCells(const std::vector<unsigned int> & cells);

    bool Start();

    void Abort();
    void InstantAbort();

    void Update(float delta);

private:
    bool InitNextMove();

    void UpdatePathCost();

    bool Fail();
    bool Finish();

private:
    std::vector<unsigned int> mCells;

    GameObject * mObj = nullptr;

    IsoMap * mIsoMap = nullptr;
    GameMap * mGameMap = nullptr;

    ScreenGame * mScreen = nullptr;

    PathState mState = READY;

    unsigned int mNextCell = 0;

    float mObjX = 0.f;
    float mObjY = 0.f;

    float mVelX = 0.f;
    float mVelY = 0.f;

    float mTargetX = 0.f;
    float mTargetY = 0.f;

    float mCost = 0.f;
};

inline GameObject * ObjectPath::GetObject() const { return mObj; }

inline ObjectPath::PathState ObjectPath::GetState() const { return mState; }

inline float ObjectPath::GetPathCost() const { return mCost; }

inline void ObjectPath::SetPathCells(const std::vector<unsigned int> & cells)
{
    mCells = cells;
    UpdatePathCost();
}

inline void ObjectPath::Abort()
{
    mState = ABORTING;
}

} // namespace game
