#pragma once

#include <vector>

namespace game
{

class IsoLayer;
class ConquestIndicator;

enum PlayerFaction : unsigned int;

class CellConquestOverlay
{
public:
    CellConquestOverlay(IsoLayer * layer, PlayerFaction faction, int mapCols);
    ~CellConquestOverlay();

    void ClearPath();
    void SetPath(const std::vector<unsigned int> & path, int cost = -1);

    void HideTarget();
    void ShowTarget(int row, int col);

private:
    ConquestIndicator * GetNewIndicator();

private:
    std::vector<ConquestIndicator *> mActiveIndicators;
    std::vector<ConquestIndicator *> mIndicators;

    ConquestIndicator * mTarget = nullptr;

    IsoLayer * mLayer = nullptr;

    PlayerFaction mFaction;

    unsigned int mNextIndicator = 0;

    int mMapCols = 0;
};

} // namespace game
