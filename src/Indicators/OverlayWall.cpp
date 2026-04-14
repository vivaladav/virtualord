#include "Indicators/OverlayWall.h"

#include "IsoLayer.h"
#include "AI/ConquerPath.h"
#include "Indicators/WallIndicator.h"
#include "Widgets/PanelUnitEnergyUsage.h"
#include "Widgets/PanelUnitResourcesUsage.h"

namespace game
{

OverlayWall::OverlayWall(IsoLayer * layer, PlayerFaction faction, int mapCols)
    : mTarget(new WallIndicator(faction))
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

OverlayWall::~OverlayWall()
{
    delete mTarget;

    for(auto pi : mActiveIndicators)
        delete pi;

    for(auto pi : mAvailableIndicators)
        delete pi;
}

bool OverlayWall::IsIndexInWallPath(unsigned int ind) const
{
    auto it = std::find(mWallPath.begin(), mWallPath.end(), ind);

    return it != mWallPath.end();
}

void OverlayWall::ClearTempPath(Unit * unit, GameMap * gm, const Cell2D & currCell,
                                        bool showTarget)
{
    if(!IsWallPathEmpty())
    {
        // clear current path
        ConquerPath cp(unit, gm, nullptr);
        cp.SetPathCells(mWallPath);

        // mark overaly as valid
        // NOTE do it before setting the path
        const int currInd = (currCell.row * mMapCols) + currCell.col;
        const bool onLast = currInd == mWallPath.back();
        SetValid(onLast);

        SetPath(mWallPath, cp.GetCostUnitEnergy(), cp.GetCostResourceEnergy(),
                cp.GetCostResourceMaterial());
        SetCostsDoable(true, true, true);
    }

    // show invalid target
    if(showTarget)
        ShowTarget(currCell.row, currCell.col, false);
    else
        HideTarget();
}

void OverlayWall::ClearPath()
{
    // clear cell start
    mCellStart.row = -1;
    mCellStart.col = -1;

    // clear full conquest path
    mWallPath.clear();

    ResetPath();
}

void OverlayWall::PopFrontPath()
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

void OverlayWall::SetPath(const std::vector<unsigned int> & path,
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

void OverlayWall::SetCostsDoable(bool unitEnergy, bool resEnergy, bool resMaterial)
{
    mPanelCost->SetDoable(unitEnergy, resEnergy, resMaterial);

    const bool doable = mPanelCost->IsDoable();

    for(auto ind : mActiveIndicators)
        ind->SetDoable(doable);
}

bool OverlayWall::IsDoable() const { return mPanelCost->IsDoable(); }
bool OverlayWall::IsDoableUnit() const { return mPanelCost->IsDoableUnit(); }
bool OverlayWall::IsDoableResources() const { return mPanelCost->IsDoableResources(); }

void OverlayWall::SetCostMoveDoable(bool doable)
{
    mPanelMoveCost->SetDoable(doable);
}

void OverlayWall::HidePanelCost()
{
    mPanelCost->SetVisible(false);
}

void OverlayWall::HideTarget()
{
    mLayer->RemoveObject(mTarget);

    mPanelCost->SetVisible(false);
    mPanelMoveCost->SetVisible(false);
}

void OverlayWall::ShowTarget(int row, int col, bool valid)
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

bool OverlayWall::IsTargetVisible() const
{
    return mTarget->IsVisible();
}

void OverlayWall::SetValid(bool valid)
{
    if(mValid == valid)
        return;

    mValid = valid;

    if(!mValid)
        mPanelCost->SetVisible(false);
}

WallIndicator * OverlayWall::GetNewIndicator()
{
    WallIndicator * pi = nullptr;

    // create new indicator
    if(mAvailableIndicators.empty())
        pi = new WallIndicator(mFaction);
    else
    // reuse existing indicator
    {
        pi = mAvailableIndicators.back();
        mAvailableIndicators.pop_back();
    }

    return pi;
}

void OverlayWall::ResetPath()
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
