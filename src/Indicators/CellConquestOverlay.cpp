#include "Indicators/CellConquestOverlay.h"

#include "IsoLayer.h"
#include "Indicators/ConquestIndicator.h"
#include "Widgets/PanelUnitEnergyUsage.h"
#include "Widgets/PanelUnitResourcesUsage.h"

namespace game
{

CellConquestOverlay::CellConquestOverlay(IsoLayer * layer, PlayerFaction faction, int mapCols)
    : mTarget(new ConquestIndicator(faction))
    , mLayer(layer)
    , mPanelCost(new PanelUnitResourcesUsage)
    , mPanelMoveCost(new PanelUnitEnergyUsage)
    , mFaction(faction)
    , mMapCols(mapCols)
{
    mPanelCost->SetVisible(false);
    mPanelMoveCost->SetVisible(false);
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

    mPanelCost->SetVisible(false);
    mPanelMoveCost->SetVisible(false);
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

void CellConquestOverlay::SetPath(const std::vector<unsigned int> & path,
                                  int costUnitEnergy, int costResEnergy, int costResMaterial)
{
    // empty path -> nothing to do
    if(path.empty())
        return ;

    // remove target
    mLayer->RemoveObject(mTarget);

    // clear existing indicators
    ClearPath();

    // create new indicators
    const bool doable = mPanelCost->IsDoable();
    const unsigned int pathSize = path.size();

    for(unsigned int i = 0; i < pathSize; ++i)
    {
        const unsigned int ind = path[i];
        const unsigned int row = ind / mMapCols;
        const unsigned int col = ind % mMapCols;

        auto pi = GetNewIndicator();
        pi->SetDoable(doable);
        mActiveIndicators.emplace_back(pi);

        mLayer->AddObject(pi, row, col);
    }

    if(costUnitEnergy > 0 && mValid)
    {
        mPanelMoveCost->SetVisible(false);

        mPanelCost->SetValues(mCostUnitMove + costUnitEnergy, costResEnergy, costResMaterial);
        mPanelCost->SetVisible(true);

        auto last = mActiveIndicators.back();

        const int x = last->GetX() + (last->GetWidth() - mPanelCost->GetWidth()) / 2;
        const int y = last->GetY() - mPanelCost->GetHeight();
        mPanelCost->SetPosition(x, y);
    }
    else
    {
        mPanelCost->SetVisible(false);
        mPanelMoveCost->SetVisible(true);
    }
}

void CellConquestOverlay::SetCostsDoable(bool unitEnergy, bool resEnergy, bool resMaterial)
{
    mPanelCost->SetDoable(unitEnergy, resEnergy, resMaterial);

    const bool doable = mPanelCost->IsDoable();

    for(auto ind : mActiveIndicators)
        ind->SetDoable(doable);
}

bool CellConquestOverlay::IsDoable() const
{
    return mPanelCost->IsDoable();
}

void CellConquestOverlay::SetCostMoveDoable(bool doable)
{
    mPanelMoveCost->SetDoable(doable);
}

void CellConquestOverlay::HidePanelCost()
{
    mPanelCost->SetVisible(false);
}

void CellConquestOverlay::HideTarget()
{
    mLayer->SetObjectVisible(mTarget, false);

    mPanelCost->SetVisible(false);
    mPanelMoveCost->SetVisible(false);
}

void CellConquestOverlay::ShowTarget(int row, int col, bool valid)
{
    if(mLayer->HasObject(mTarget))
    {
        mLayer->MoveObject(mTarget, row, col);
        mLayer->SetObjectVisible(mTarget, true);
    }
    // indicator not visible yet
    else
    {
        mTarget->SetVisible(true);
        mLayer->AddObject(mTarget, row, col);
    }

    const int alpha = valid ? 255 : 150;
    mTarget->SetAlpha(alpha);

    if(mActiveIndicators.empty())
    {
        mPanelMoveCost->SetVisible(true);
        mPanelMoveCost->SetValue(mCostUnitMove);

        const int x = mTarget->GetX() + (mTarget->GetWidth() - mPanelMoveCost->GetWidth()) / 2;
        const int y = mTarget->GetY() - mPanelMoveCost->GetHeight();
        mPanelMoveCost->SetPosition(x, y);
    }
    else
        mPanelMoveCost->SetVisible(false);
}

bool CellConquestOverlay::IsTargetVisible() const
{
    return mTarget->IsVisible();
}

void CellConquestOverlay::SetValid(bool valid)
{
    if(mValid == valid)
        return;

    mValid = valid;

    if(!mValid)
        mPanelCost->SetVisible(false);
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
