#include "Indicators/CellConquestOverlay.h"

#include "GameConstants.h"
#include "IsoLayer.h"
#include "Indicators/ConquestIndicator.h"

namespace game
{

CellConquestOverlay::CellConquestOverlay(IsoLayer * layer, PlayerFaction faction, int mapCols)
    : mTarget(new ConquestIndicator(faction))
    , mLayer(layer)
    , mFaction(faction)
    , mMapCols(mapCols)
{
}

CellConquestOverlay::~CellConquestOverlay()
{
    delete mTarget;

    for(auto pi : mIndicators)
        delete pi;
}

void CellConquestOverlay::ClearPath()
{
    for(auto pi : mActiveIndicators)
        mLayer->ClearObject(pi);

    mActiveIndicators.clear();

    mNextIndicator = 0;
}

void CellConquestOverlay::SetPath(const std::vector<unsigned int> & path, int cost)
{
    // empty path -> nothing to do
    if(path.empty())
        return ;

    // remove target
    mLayer->ClearObject(mTarget);

    // clear existing indicators
    ClearPath();

    // create new indicators
    const unsigned int pathSize = path.size();

    for(unsigned int i = 0; i < pathSize; ++i)
    {
        const unsigned int ind = path[i];
        const unsigned int row = ind / mMapCols;
        const unsigned int col = ind % mMapCols;

        auto pi = GetNewIndicator();
        mActiveIndicators.emplace_back(pi);

        mLayer->AddObject(pi, row, col);
    }

    mActiveIndicators.back()->SetCost(cost);
}

void CellConquestOverlay::HideTarget()
{
    mLayer->SetObjectVisible(mTarget, false);
}

void CellConquestOverlay::ShowTarget(int row, int col)
{
    if(mLayer->HasObject(mTarget))
    {
        mLayer->MoveObject(mTarget, row, col);
        mLayer->SetObjectVisible(mTarget, true);
    }
    // indicator not visible yet
    else
        mLayer->AddObject(mTarget, row, col);
}

ConquestIndicator * CellConquestOverlay::GetNewIndicator()
{
    ConquestIndicator * pi = nullptr;

    // reuse existing indicator
    if(mNextIndicator < mIndicators.size())
    {
        pi = mIndicators[mNextIndicator];
    }
    // create new one
    else
    {
        pi = new ConquestIndicator(mFaction);

        mIndicators.emplace_back(pi);
    }

    ++mNextIndicator;

    return pi;
}

} // namespace game
