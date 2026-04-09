#pragma once

#include <vector>

namespace game
{

class IsoLayer;
class PanelUnitEnergyUsage;
class PathIndicator;

enum PlayerFaction : unsigned int;

class PathOverlay
{
public:
    PathOverlay(IsoLayer * layer, PlayerFaction faction, int mapCols);
    ~PathOverlay();

    void ClearPath();
    void SetPath(const std::vector<unsigned int> & path, int cost = -1, bool doable = true);

    void HideTarget();
    void ShowTarget(int row, int col);

private:
    PathIndicator * GetNewIndicator(bool doable, bool final);

private:
    std::vector<PathIndicator *> mActiveIndicators;
    std::vector<PathIndicator *> mAvailableIndicators;

    PathIndicator * mPathTarget = nullptr;

    IsoLayer * mLayer = nullptr;

    PanelUnitEnergyUsage * mPanelCost = nullptr;

    PlayerFaction mFaction;

    int mMapCols = 0;
};

} // namespace game
