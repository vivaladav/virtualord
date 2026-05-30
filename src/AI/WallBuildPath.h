#pragma once

#include "GameObjects/GameObjectTypes.h"

#include <vector>

namespace game
{

class GameMap;
class GameMapProgressBar;
class IsoMap;
class OverlayWall;
class ScreenGame;
class Unit;

class WallBuildPath
{
public:
    enum BuildState : unsigned int
    {
        READY,
        BUILDING,
        MOVING,
        START_NEXT,
        COMPLETED,
        FAILED,
        ABORTING,
        ABORTED,

        NUM_PATH_STATES
    };

public:
    WallBuildPath(Unit * unit, IsoMap * im, GameMap * gm, ScreenGame * sg,
                  OverlayWall * ov = nullptr);
    ~WallBuildPath();

    Unit * GetUnit() const;

    BuildState GetState() const;

    int GetCostUnitEnergy() const;
    int GetCostResourceEnergy() const;
    int GetCostResourceMaterial() const;

    void SetPath(const std::vector<unsigned int> & cells);

    unsigned int GetWallLevel() const;
    void SetWallLevel(unsigned int level);

    bool HasStarted() const;
    bool Start();

    void Abort();
    void InstantAbort();

    void Update(float delta);

private:
    void SetIndicatorsType();

    bool InitNextBuild();
    bool InitNextMove();

    void UpdateMove(float delta);

    void UpdatePathCost();

    bool Fail();
    bool Finish();

    int IndToRow(unsigned int ind) const;
    int IndToCol(unsigned int ind) const;

private:
    std::vector<unsigned int> mCells;
    std::vector<GameObjectVariantId> mBlockTypes;

    OverlayWall * mOverlay = nullptr;

    GameMapProgressBar * mProgressBar = nullptr;

    Unit * mUnit = nullptr;

    IsoMap * mIsoMap = nullptr;
    GameMap * mGameMap = nullptr;

    ScreenGame * mScreen = nullptr;

    BuildState mState = READY;

    unsigned int mNextCell = 0;

    float mObjX = 0.f;
    float mObjY = 0.f;

    float mVelX = 0.f;
    float mVelY = 0.f;

    float mTargetX = 0.f;
    float mTargetY = 0.f;
    unsigned int mTargetRow = 0;
    unsigned int mTargetCol = 0;

    int mCostUnitEnergy = 0;
    int mCostResEnergy = 0.f;
    int mCostResMaterial = 0.f;

    unsigned int mLevel = 0;

    bool mLocal = false;
};

inline Unit * WallBuildPath::GetUnit() const { return mUnit; }

inline WallBuildPath::BuildState WallBuildPath::GetState() const { return mState; }

inline int WallBuildPath::GetCostUnitEnergy() const { return mCostUnitEnergy; }
inline int WallBuildPath::GetCostResourceEnergy() const { return mCostResEnergy; }
inline int WallBuildPath::GetCostResourceMaterial() const { return mCostResMaterial; }

inline unsigned int WallBuildPath::GetWallLevel() const { return mLevel; }
inline void WallBuildPath::SetWallLevel(unsigned int level) { mLevel = level; }

inline bool WallBuildPath::HasStarted() const { return mState != READY; }

} // namespace game
