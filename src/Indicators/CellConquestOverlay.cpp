#include "Indicators/CellConquestOverlay.h"

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

    for(auto pi : mActiveIndicators)
        delete pi;

    for(auto pi : mAvailableIndicators)
        delete pi;
}

void CellConquestOverlay::ClearPath()
{
    for(auto pi : mActiveIndicators)
    {
        mLayer->RemoveObject(pi);

        mAvailableIndicators.emplace_back(pi);
    }

    mActiveIndicators.clear();
}

void CellConquestOverlay::PopFrontPath()
{
    if(mActiveIndicators.empty())
        return ;

    // remove front from active indicators
    auto ind = mActiveIndicators.front();
    mActiveIndicators.pop_front();

    // remove indicator from layer
    mLayer->RemoveObject(ind);

    // add indicator back to available ones
    mAvailableIndicators.emplace_back(ind);
}

void CellConquestOverlay::SetPath(const std::vector<unsigned int> & path, int cost)
{
    // empty path -> nothing to do
    if(path.empty())
        return ;

    // remove target
    mLayer->RemoveObject(mTarget);

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

    // create new indicator
    if(mAvailableIndicators.empty())
        pi = new ConquestIndicator(mFaction);
    else
    // reuse existing indicator
    {
        pi = mAvailableIndicators.back();
        mAvailableIndicators.pop_back();
    }

    return pi;
}

} // namespace game
