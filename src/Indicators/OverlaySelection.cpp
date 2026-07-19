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

void OverlaySelection::AddObject(GameObject * obj, bool primary)
{
    const int r0 = obj->GetRow0();
    const int c0 = obj->GetCol0();

    // cell already occupied -> exit
    if(mLayer->HasObject(r0, c0))
        return ;

    const int objSize = obj->GetRows() * obj->GetCols();

    // multiple cells object
    if(objSize > 1)
    {
        const int r1 = obj->GetRow1();
        const int c1 = obj->GetCol1();

        auto indTL = GetNewIndicator(IndicatorSelection::SEL_IND_TL, primary);
        auto indTR = GetNewIndicator(IndicatorSelection::SEL_IND_TR, primary);
        auto indBL = GetNewIndicator(IndicatorSelection::SEL_IND_BL, primary);
        auto indBR = GetNewIndicator(IndicatorSelection::SEL_IND_BR, primary);

        mLayer->AddObject(indTL, r1, c1);
        mLayer->AddObject(indTR, r1, c0);
        mLayer->AddObject(indBL, r0, c1);
        mLayer->AddObject(indBR, r0, c0);

        mAreaIndicators.emplace(obj, MultiIndicator(indTL, indTR, indBL, indBR));
    }
    // single cell object
    else
    {
        auto ind = GetNewIndicator(IndicatorSelection::SEL_IND_CELL, primary);

        mLayer->AddObject(ind, r0, c0);

        mCellIndicators.emplace(obj, ind);
    }
}

void OverlaySelection::UpdateObjectPosition(GameObject * obj)
{
    const int objSize = obj->GetRows() * obj->GetCols();

    // multiple cells object
    if(objSize > 1)
    {
        auto it = mAreaIndicators.find(obj);

        if(it == mAreaIndicators.end())
            return;

        const MultiIndicator & mi = it->second;

        auto isoObj = obj->GetIsoObject();

        const int x0 = isoObj->GetX();
        const int y0 = isoObj->GetY();
        const int objW = isoObj->GetWidth();
        const int objH = isoObj->GetHeight();
        const int indW = mi.indTL->GetWidth();
        const int indH = mi.indTL->GetHeight();

        int x = 0;
        int y = 0;

        // TOP-LEFT
        x = x0 + (objW - indW) / 2;
        y = y0;
        mi.indTL->SetPosition(x, y);

        // TOP-RIGHT
        x = x0 + objW - indW;
        y = y0 + (objH - indH) / 2;
        mi.indTR->SetPosition(x, y);

        // BOTTOM-LEFT
        x = x0;
        y = y0 + (objH - indH) / 2;
        mi.indBL->SetPosition(x, y);

        // BOTTOM-RIGHT
        x = x0 + (objW - indW) / 2;
        y = y0 + objH - indH;
        mi.indBR->SetPosition(x, y);
    }
    // single cell object
    else
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
}

void OverlaySelection::UpdateObjectCell(GameObject * obj)
{
    const int objSize = obj->GetRows() * obj->GetCols();

    // multiple cells object
    if(objSize > 1)
    {
        auto it = mAreaIndicators.find(obj);

        if(it == mAreaIndicators.end())
            return;

        const MultiIndicator & mi = it->second;

        const int r0 = obj->GetRow0();
        const int c0 = obj->GetCol0();
        const int r1 = obj->GetRow1();
        const int c1 = obj->GetCol1();

        mLayer->AddObject(mi.indTL, r1, c1);
        mLayer->AddObject(mi.indTR, r1, c0);
        mLayer->AddObject(mi.indBL, r0, c1);
        mLayer->AddObject(mi.indBR, r0, c0);
    }
    // single cell object
    else
    {
        auto it = mCellIndicators.find(obj);

        if(it == mCellIndicators.end())
            return;

        auto ind = it->second;

        mLayer->MoveObject(ind, obj->GetRow0(), obj->GetCol0());
    }
}

IndicatorSelection * OverlaySelection::GetNewIndicator(unsigned int type, bool primary)
{
    IndicatorSelection * ind = nullptr;

    // create new indicator
    if(mAvailableIndicators.empty())
        ind = new IndicatorSelection(static_cast<IndicatorSelection::IndicatorType>(type), primary);
    else
    // reuse existing indicator
    {
        ind = mAvailableIndicators.back();
        mAvailableIndicators.pop_back();

        ind->SetType(static_cast<IndicatorSelection::IndicatorType>(type));
        ind->SetPrimary(primary);
    }

    return ind;
}

} // namespace game