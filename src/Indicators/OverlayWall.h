#pragma once

#include "Cell2D.h"

#include <deque>
#include <vector>

namespace game
{

class GameMap;
class IsoLayer;
class PanelUnitEnergyUsage;
class PanelUnitResourcesUsage;
class WallIndicator;
class Unit;

enum PlayerFaction : unsigned int;

class OverlayWall
{
public:
    OverlayWall(IsoLayer * layer, PlayerFaction faction, int mapCols);
    ~OverlayWall();

    const Cell2D & GetCellStart() const;
    void SetCellStart(int row, int col);
    void SetCellStart(const Cell2D & start);
    bool IsCellStartSet() const;

    const std::vector<unsigned int> & GetWallPath() const;
    bool IsWallPathEmpty() const;
    bool IsIndexInWallPath(unsigned int ind) const;
    unsigned int GetWallPathSize() const;
    unsigned int GetWallPathBack() const;
    void AddPathToWallPath(const std::vector<unsigned int> & path);

    void ClearTempPath(Unit * unit, GameMap * gm, const Cell2D & currCell, bool showTarget);

    void ClearPath();
    void PopFrontPath();
    void SetPath(const std::vector<unsigned int> & path,
                 int costUnitEnergy = -1, int costResEnergy = -1, int costResMaterial = -1);
    void SetCostsDoable(bool unitEnergy, bool resEnergy, bool resMaterial);
    bool IsDoable() const;
    bool IsDoableUnit() const;
    bool IsDoableResources() const;
    void SetCostEnergyUnitMove(int cost);
    int GetCostEnergyUnitMove() const;
    void SetCostMoveDoable(bool doable);

    void HidePanelCost();

    void HideTarget();
    void ShowTarget(int row, int col, bool valid = true);
    bool IsTargetVisible() const;

    bool IsValid() const;
    void SetValid(bool valid);

private:
    WallIndicator * GetNewIndicator();

    void ResetPath();

    void SetIndicatorsType();

private:
    std::deque<WallIndicator *> mActiveIndicators;
    std::deque<WallIndicator *> mAvailableIndicators;

    std::vector<unsigned int> mWallPath;

    Cell2D mCellStart;

    WallIndicator * mTarget = nullptr;

    IsoLayer * mLayer = nullptr;

    PanelUnitResourcesUsage * mPanelCost = nullptr;
    PanelUnitEnergyUsage * mPanelMoveCost = nullptr;

    PlayerFaction mFaction;

    int mMapCols = 0;
    int mCostUnitMove = 0;

    bool mValid = true;
};

inline const Cell2D & OverlayWall::GetCellStart() const { return mCellStart; }
inline void OverlayWall::SetCellStart(int row, int col)
{
    mCellStart.row = row;
    mCellStart.col = col;
}
inline void OverlayWall::SetCellStart(const Cell2D & start) { mCellStart = start; }
inline bool OverlayWall::IsCellStartSet() const
{
    return mCellStart.row >= 0 && mCellStart.col >= 0;
}

inline const std::vector<unsigned int> & OverlayWall::GetWallPath() const
{
    return mWallPath;
}

inline bool OverlayWall::IsWallPathEmpty() const
{
    return mWallPath.empty();
}
inline unsigned int OverlayWall::GetWallPathSize() const { return mWallPath.size(); }
inline unsigned int OverlayWall::GetWallPathBack() const { return mWallPath.back(); }
inline void OverlayWall::AddPathToWallPath(const std::vector<unsigned int> & path)
{
    mWallPath.reserve(mWallPath.size() + path.size());
    mWallPath.insert(mWallPath.end(), path.begin(), path.end());
}

inline void OverlayWall::SetCostEnergyUnitMove(int cost) { mCostUnitMove = cost; }
inline int OverlayWall::GetCostEnergyUnitMove() const { return mCostUnitMove; }

inline bool OverlayWall::IsValid() const { return mValid; }

} // namespace game
