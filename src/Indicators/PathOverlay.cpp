#include "Indicators/PathOverlay.h"

#include "GameConstants.h"
#include "IsoLayer.h"
#include "Indicators/PathIndicator.h"

#include <cassert>

namespace game
{

PathOverlay::PathOverlay(IsoLayer * layer, PlayerFaction faction, int mapRows, int mapCols)
    : mPathTarget(new PathIndicator(faction, true))
    , mLayer(layer)
    , mFaction(faction)
    , mMapRows(mapRows)
    , mMapCols(mapCols)
{
}

PathOverlay::~PathOverlay()
{
    delete mPathTarget;

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

void PathOverlay::SetPath(const std::vector<unsigned int> & path, int cost, bool doable)
{
    // empty path -> nothing to do
    if(path.empty())
        return ;

    assert(path.size() > 1);

    // remove target
    mLayer->ClearObject(mPathTarget);

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

        auto pi = GetNewIndicator(doable, i == lastInd);
        mActiveIndicators.emplace_back(pi);

        mLayer->AddObject(pi, row, col);
    }

    mActiveIndicators.back()->SetCost(cost);
}

void PathOverlay::HideTarget()
{
    mLayer->SetObjectVisible(mPathTarget, false);
}

void PathOverlay::ShowTarget(int row, int col)
{
    if(mLayer->HasObject(mPathTarget))
    {
        mLayer->MoveObject(mPathTarget, row, col);
        mLayer->SetObjectVisible(mPathTarget, true);
    }
    // indicator not visible yet
    else
        mLayer->AddObject(mPathTarget, row, col);
}

PathIndicator * PathOverlay::GetNewIndicator(bool doable, bool final)
{
    PathIndicator * pi = nullptr;

    // reuse existing indicator
    if(mNextIndicator < mIndicators.size())
    {
        pi = mIndicators[mNextIndicator];

        pi->SetFaction(mFaction);
        pi->SetFinal(final);
        pi->ClearCost();
    }
    // create new one
    else
    {
        pi = new PathIndicator(mFaction, final);

        mIndicators.emplace_back(pi);
    }

    ++mNextIndicator;

    pi->SetDoable(doable);

    return pi;
}

} // namespace game
