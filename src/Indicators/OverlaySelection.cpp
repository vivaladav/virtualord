#include "Indicators/OverlaySelection.h"

#include "IsoLayer.h"
#include "IsoObject.h"
#include "GameObjects/GameObject.h"
#include "Indicators/IndicatorSelection.h"

namespace game
{

OverlaySelection::OverlaySelection(IsoLayer * layer)
    : mLayer(layer)
{

}

OverlaySelection::~OverlaySelection()
{

}

void OverlaySelection::ClearIndicators()
{
    mLayer->ClearObjects();

    mAvailableIndicators.insert(mAvailableIndicators.end(),
                                mActiveIndicators.begin(), mActiveIndicators.end());

    mActiveIndicators.clear();

    mCellIndicators.clear();
}

void OverlaySelection::AddCellIndicator(GameObject * obj)
{
    const int r0 = obj->GetRow0();
    const int c0 = obj->GetCol0();

    if(mLayer->HasObject(r0, c0))
        return ;

    auto ind = GetNewIndicator();

    mLayer->AddObject(ind, r0, c0);

    mCellIndicators.emplace(obj, ind);
}

void OverlaySelection::UpdateIndicatorPosition(GameObject * obj)
{
    auto it = mCellIndicators.find(obj);

    if(it == mCellIndicators.end())
        return;

    auto ind = it->second;
    auto isoObj = obj->GetIsoObject();

    const int x = isoObj->GetX();
    const int y = isoObj->GetY() + isoObj->GetHeight() - ind->GetHeight();

    //const int pos
    ind->SetPosition(x, y);
}

void OverlaySelection::UpdateIndicatorCell(GameObject * obj)
{
    auto it = mCellIndicators.find(obj);

    if(it == mCellIndicators.end())
        return;

    auto ind = it->second;

    mLayer->MoveObject(ind, obj->GetRow0(), obj->GetCol0());
}

IndicatorSelection * OverlaySelection::GetNewIndicator()
{
    IndicatorSelection * ind = nullptr;

    // create new indicator
    if(mAvailableIndicators.empty())
        ind = new IndicatorSelection(IndicatorSelection::SEL_IND_CELL);
    else
    // reuse existing indicator
    {
        ind = mAvailableIndicators.back();
        mAvailableIndicators.pop_back();

        ind->SetType(IndicatorSelection::SEL_IND_CELL);
    }

    return ind;
}

} // namespace game