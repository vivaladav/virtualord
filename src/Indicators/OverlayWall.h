#pragma once

#include <deque>
#include <vector>

namespace game
{

class IsoLayer;
class WallIndicator;
class PanelUnitEnergyUsage;
class PanelUnitResourcesUsage;

enum PlayerFaction : unsigned int;

class OverlayWall
{
public:
    OverlayWall(IsoLayer * layer, PlayerFaction faction, int mapCols);
    ~OverlayWall();

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

private:
    std::deque<WallIndicator *> mActiveIndicators;
    std::deque<WallIndicator *> mAvailableIndicators;

    WallIndicator * mTarget = nullptr;

    IsoLayer * mLayer = nullptr;

    PanelUnitResourcesUsage * mPanelCost = nullptr;
    PanelUnitEnergyUsage * mPanelMoveCost = nullptr;

    PlayerFaction mFaction;

    int mMapCols = 0;
    int mCostUnitMove = 0;

    bool mValid = true;
};

inline void OverlayWall::SetCostEnergyUnitMove(int cost) { mCostUnitMove = cost; }
inline int OverlayWall::GetCostEnergyUnitMove() const { return mCostUnitMove; }

inline bool OverlayWall::IsValid() const { return mValid; }

} // namespace game
