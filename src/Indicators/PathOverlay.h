#pragma once

#include <vector>

namespace game
{

class IsoLayer;
class PathIndicator;

enum PlayerFaction : unsigned int;

class PathOverlay
{
public:
    PathOverlay(IsoLayer * layer, PlayerFaction faction, int mapRows, int mapCols);
    ~PathOverlay();

    void ClearPath();
    void SetPath(const std::vector<unsigned int> & path, int cost = -1, bool doable = true);

    void HideTarget();
    void ShowTarget(int row, int col);

private:
    PathIndicator * GetNewIndicator(bool doable, bool final);

private:
    std::vector<PathIndicator *> mActiveIndicators;
    std::vector<PathIndicator *> mIndicators;

    PathIndicator * mPathTarget = nullptr;

    IsoLayer * mLayer = nullptr;

    PlayerFaction mFaction;

    unsigned int mNextIndicator = 0;

    int mMapRows = 0;
    int mMapCols = 0;
};

} // namespace game
