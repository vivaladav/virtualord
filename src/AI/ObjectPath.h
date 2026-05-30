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
    bool IsTerminated() const;

    int GetPathCost() const;

    // CURRENT PATH
    // NOTE path is expected to include start position
    const std::vector<unsigned int> & GetPath() const;
    void SetPath(const std::vector<unsigned int> & cells);

    unsigned int GetLastStepDone() const;

    bool HasStarted() const;
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

    int mCost = 0;

    bool mLocal = false;
};

inline GameObject * ObjectPath::GetObject() const { return mObj; }

inline ObjectPath::PathState ObjectPath::GetState() const { return mState; }

inline bool ObjectPath::IsTerminated() const
{
    return mState == COMPLETED || mState == ABORTED || mState == FAILED;
}

inline unsigned int ObjectPath::GetLastStepDone() const
{
    if(mNextCell > 0)
        return mNextCell - 1;
    else
        return 0;
}

inline bool ObjectPath::HasStarted() const { return mState != READY; }

inline int ObjectPath::GetPathCost() const { return mCost; }

inline const std::vector<unsigned int> & ObjectPath::GetPath() const { return mCells; }

inline void ObjectPath::SetPath(const std::vector<unsigned int> & cells)
{
    mCells = cells;
    UpdatePathCost();
}

inline void ObjectPath::Abort()
{
    mState = ABORTING;
}

} // namespace game
