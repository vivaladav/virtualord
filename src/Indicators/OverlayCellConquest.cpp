#include "Indicators/OverlayCellConquest.h"

#include "IsoLayer.h"
#include "AI/ConquerPath.h"
#include "Indicators/ConquestIndicator.h"
#include "Widgets/PanelUnitEnergyUsage.h"
#include "Widgets/PanelUnitResourcesUsage.h"

namespace game
{

OverlayCellConquest::OverlayCellConquest(IsoLayer * layer, PlayerFaction faction, int mapCols)
    : mTarget(new ConquestIndicator(faction))
    , mLayer(layer)
    , mPanelCost(new PanelUnitResourcesUsage)
    , mPanelMoveCost(new PanelUnitEnergyUsage)
    , mFaction(faction)
    , mMapCols(mapCols)
{
    mPanelCost->SetVisible(false);
    mPanelMoveCost->SetVisible(false);

    mCellStart.row = -1;
    mCellStart.col = -1;
}

OverlayCellConquest::~OverlayCellConquest()
{
    delete mTarget;

    for(auto pi : mActiveIndicators)
        delete pi;

    for(auto pi : mAvailableIndicators)
        delete pi;
}

bool OverlayCellConquest::IsIndexInConquestPath(unsigned int ind) const
{
    auto it = std::find(mConquestPath.begin(), mConquestPath.end(), ind);

    return it != mConquestPath.end();
}

void OverlayCellConquest::ClearTempPath(Unit * unit, GameMap * gm, const Cell2D & currCell,
                                        bool showTarget)
{
    if(!IsConquestPathEmpty())
    {
        // clear current path
        ConquerPath cp(unit, gm, nullptr);
        cp.SetPathCells(mConquestPath);

        // mark overaly as valid
        // NOTE do it before setting the path
        const int currInd = (currCell.row * mMapCols) + currCell.col;
        const bool onLast = currInd == mConquestPath.back();
        SetValid(onLast);

        SetPath(mConquestPath, cp.GetCostUnitEnergy(), cp.GetCostResourceEnergy(),
                cp.GetCostResourceMaterial());
        SetCostsDoable(true, true, true);
    }

    // show invalid target
    if(showTarget)
        ShowTarget(currCell.row, currCell.col, false);
    else
        HideTarget();
}

void OverlayCellConquest::ClearPath()
{
    // clear cell start
    mCellStart.row = -1;
    mCellStart.col = -1;

    // clear full conquest path
    mConquestPath.clear();

    ResetPath();
}

void OverlayCellConquest::PopFrontPath()
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

void OverlayCellConquest::SetPath(const std::vector<unsigned int> & path,
                                  int costUnitEnergy, int costResEnergy, int costResMaterial)
{
    // empty path -> nothing to do
    if(path.empty())
        return ;

    // remove target
    mLayer->RemoveObject(mTarget);

    // clear existing indicators
    ResetPath();

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

void OverlayCellConquest::SetCostsDoable(bool unitEnergy, bool resEnergy, bool resMaterial)
{
    mPanelCost->SetDoable(unitEnergy, resEnergy, resMaterial);

    const bool doable = mPanelCost->IsDoable();

    for(auto ind : mActiveIndicators)
        ind->SetDoable(doable);
}

bool OverlayCellConquest::IsDoable() const { return mPanelCost->IsDoable(); }
bool OverlayCellConquest::IsDoableUnit() const { return mPanelCost->IsDoableUnit(); }
bool OverlayCellConquest::IsDoableResources() const { return mPanelCost->IsDoableResources(); }

void OverlayCellConquest::SetCostMoveDoable(bool doable)
{
    mPanelMoveCost->SetDoable(doable);
}

void OverlayCellConquest::HidePanelCost()
{
    mPanelCost->SetVisible(false);
}

void OverlayCellConquest::HideTarget()
{
    mLayer->RemoveObject(mTarget);

    mPanelCost->SetVisible(false);
    mPanelMoveCost->SetVisible(false);
}

void OverlayCellConquest::ShowTarget(int row, int col, bool valid)
{
    if(mLayer->HasObject(mTarget))
    {
        mLayer->MoveObject(mTarget, row, col);
        mLayer->SetObjectVisible(mTarget, true);
    }
    // indicator not visible yet
    else
        mLayer->AddObject(mTarget, row, col);

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

bool OverlayCellConquest::IsTargetVisible() const
{
    return mTarget->IsVisible();
}

void OverlayCellConquest::SetValid(bool valid)
{
    if(mValid == valid)
        return;

    mValid = valid;

    if(!mValid)
        mPanelCost->SetVisible(false);
}

ConquestIndicator * OverlayCellConquest::GetNewIndicator()
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

void OverlayCellConquest::ResetPath()
{
    // clear active indicators
    for(auto pi : mActiveIndicators)
    {
        mLayer->RemoveObject(pi);

        mAvailableIndicators.emplace_back(pi);
    }

    mActiveIndicators.clear();

    // clear panels
    mPanelCost->SetVisible(false);
    mPanelCost->SetDoable(true, true, true);
    mPanelMoveCost->SetVisible(false);
    mPanelMoveCost->SetDoable(true);
}

} // namespace game
