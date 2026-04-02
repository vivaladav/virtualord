#pragma once

#include <deque>
#include <vector>

namespace game
{

class IsoLayer;
class ConquestIndicator;
class PanelUnitResourcesUsage;

enum PlayerFaction : unsigned int;

class CellConquestOverlay
{
public:
    CellConquestOverlay(IsoLayer * layer, PlayerFaction faction, int mapCols);
    ~CellConquestOverlay();

    void ClearPath();
    void PopFrontPath();
    void SetPath(const std::vector<unsigned int> & path, int cost = -1);

    void HideTarget();
    void ShowTarget(int row, int col);

private:
    ConquestIndicator * GetNewIndicator();

private:
    std::deque<ConquestIndicator *> mActiveIndicators;
    std::deque<ConquestIndicator *> mAvailableIndicators;

    ConquestIndicator * mTarget = nullptr;

    IsoLayer * mLayer = nullptr;

    PanelUnitResourcesUsage * mPanelCost = nullptr;

    PlayerFaction mFaction;

    int mMapCols = 0;
};

} // namespace game
