#pragma once

#include "Cell2D.h"

#include <deque>
#include <vector>

namespace game
{

class IsoLayer;
class ConquestIndicator;
class PanelUnitEnergyUsage;
class PanelUnitResourcesUsage;

enum PlayerFaction : unsigned int;

class OverlayCellConquest
{
public:
    OverlayCellConquest(IsoLayer * layer, PlayerFaction faction, int mapCols);
    ~OverlayCellConquest();

    const Cell2D & GetCellStart() const;
    void SetCellStart(int row, int col);
    void SetCellStart(const Cell2D & start);
    bool IsCellStartSet() const;

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
    ConquestIndicator * GetNewIndicator();

    void ResetPath();

private:
    std::deque<ConquestIndicator *> mActiveIndicators;
    std::deque<ConquestIndicator *> mAvailableIndicators;

    Cell2D mCellStart;

    ConquestIndicator * mTarget = nullptr;

    IsoLayer * mLayer = nullptr;

    PanelUnitResourcesUsage * mPanelCost = nullptr;
    PanelUnitEnergyUsage * mPanelMoveCost = nullptr;

    PlayerFaction mFaction;

    int mMapCols = 0;
    int mCostUnitMove = 0;

    bool mValid = true;
};

inline const Cell2D & OverlayCellConquest::GetCellStart() const { return mCellStart; }
inline void OverlayCellConquest::SetCellStart(int row, int col)
{
    mCellStart.row = row;
    mCellStart.col = col;
}
inline void OverlayCellConquest::SetCellStart(const Cell2D & start) { mCellStart = start; }
inline bool OverlayCellConquest::IsCellStartSet() const
{
    return mCellStart.row >= 0 && mCellStart.col >= 0;
}

inline void OverlayCellConquest::SetCostEnergyUnitMove(int cost) { mCostUnitMove = cost; }
inline int OverlayCellConquest::GetCostEnergyUnitMove() const { return mCostUnitMove; }

inline bool OverlayCellConquest::IsValid() const { return mValid; }

} // namespace game
