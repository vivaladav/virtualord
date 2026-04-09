#include "Indicators/OverlayPath.h"

#include "GameConstants.h"
#include "IsoLayer.h"
#include "Indicators/PathIndicator.h"
#include "Widgets/PanelUnitEnergyUsage.h"

#include <cassert>

namespace game
{

PathOverlay::PathOverlay(IsoLayer * layer, PlayerFaction faction, int mapCols)
    : mPathTarget(new PathIndicator(faction, true))
    , mLayer(layer)
    , mPanelCost(new PanelUnitEnergyUsage)
    , mFaction(faction)
    , mMapCols(mapCols)
{
    mPanelCost->SetVisible(false);
}

PathOverlay::~PathOverlay()
{
    delete mPathTarget;

    for(auto pi : mActiveIndicators)
        delete pi;

    for(auto pi : mAvailableIndicators)
        delete pi;
}

void PathOverlay::ClearPath()
{
    for(auto pi : mActiveIndicators)
    {
        mLayer->RemoveObject(pi);

        mAvailableIndicators.emplace_back(pi);
    }

    mActiveIndicators.clear();

    mPanelCost->SetVisible(false);
}

void PathOverlay::SetPath(const std::vector<unsigned int> & path, int cost, bool doable)
{
    // empty path -> nothing to do
    if(path.empty())
        return ;

    assert(path.size() > 1);

    // remove target
    mLayer->RemoveObject(mPathTarget);

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

    if(cost > 0)
    {
        mPanelCost->SetValue(cost);
        mPanelCost->SetDoable(doable);
        mPanelCost->SetVisible(true);

        auto last = mActiveIndicators.back();

        const int x = last->GetX() + (last->GetWidth() - mPanelCost->GetWidth()) / 2;
        const int y = last->GetY() - mPanelCost->GetHeight();
        mPanelCost->SetPosition(x, y);
    }
    else
        mPanelCost->SetVisible(false);
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

    // create new indicator
    if(mAvailableIndicators.empty())
        pi = new PathIndicator(mFaction, final);
    // reuse existing indicator
    else
    {
        pi = mAvailableIndicators.back();
        mAvailableIndicators.pop_back();

        pi->SetFinal(final);
    }

    pi->SetDoable(doable);

    return pi;
}

} // namespace game
