#include "Indicators/PathOverlay.h"

#include "GameConstants.h"
#include "IsoLayer.h"
#include "Indicators/PathIndicator.h"

#include <cassert>

namespace game
{

PathOverlay::PathOverlay(IsoLayer * layer, int mapRows, int mapCols)
    : mLayer(layer)
    , mMapRows(mapRows)
    , mMapCols(mapCols)
{
}

PathOverlay::~PathOverlay()
{
    for(auto pi : mIndicators)
        delete pi;
}

void PathOverlay::ClearPath()
{
    for(auto pi : mActiveIndicators)
        mLayer->ClearObject(pi);

    mActiveIndicators.clear();

    mNextIndicator = 0;
}

void PathOverlay::SetPath(const std::vector<unsigned int> & path, PlayerFaction faction, int cost, bool doable)
{
    // empty path -> nothing to do
    if(path.empty())
        return ;

    assert(path.size() > 1);

    // clear existing indicators
    ClearPath();

    // create new indicators
    const unsigned int pathSize = path.size();
    const unsigned int lastInd = pathSize - 1;

    for(unsigned int i = 1; i < pathSize; ++i)
    {
        const unsigned int ind = path[i];
        const unsigned int row = ind / mMapCols;
        const unsigned int col = ind % mMapCols;

        auto pi = GetNewIndicator(faction, doable, i == lastInd);
        mActiveIndicators.emplace_back(pi);

        mLayer->AddObject(pi, row, col);
    }

    mActiveIndicators.back()->SetCost(cost);
}

PathIndicator * PathOverlay::GetNewIndicator(PlayerFaction faction, bool doable, bool final)
{
    PathIndicator * pi = nullptr;

    // reuse existing indicator
    if(mNextIndicator < mIndicators.size())
    {
        pi = mIndicators[mNextIndicator];

        pi->SetFaction(faction);
        pi->SetFinal(final);
        pi->ClearCost();
    }
    // create new one
    else
    {
        pi = new PathIndicator(faction, final);

        mIndicators.emplace_back(pi);
    }

    ++mNextIndicator;

    pi->SetDoable(doable);

    return pi;
}

} // namespace game
