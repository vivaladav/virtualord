#include "Indicators/OverlayHealRange.h"

#include "GameConstants.h"
#include "IsoLayer.h"
#include "IsoMap.h"
#include "GameObjects/GameObject.h"
#include "Indicators/HealingRangeIndicator.h"

#include <cassert>

namespace game
{

OverlayHealRange::OverlayHealRange(IsoMap * im)
    : mIsoMap(im)
    , mLayer(im->GetLayer(MapLayers::CELL_OVERLAYS0))
{
}

OverlayHealRange::~OverlayHealRange()
{
    for(auto pi : mActiveIndicators)
        delete pi;

    for(auto pi : mAvailableIndicators)
        delete pi;
}

void OverlayHealRange::Clear()
{
    for(auto pi : mActiveIndicators)
    {
        mLayer->RemoveObject(pi);

        mAvailableIndicators.emplace_back(pi);
    }

    mActiveIndicators.clear();
}

void OverlayHealRange::Show(const GameObject * obj, unsigned int range)
{
    // clear current indicators
    Clear();

    // show indicators around object
    const int rows = mIsoMap->GetNumRows();
    const int cols = mIsoMap->GetNumCols();
    const int r0 = obj->GetRow0();
    const int c0 = obj->GetCol0();
    const int r1 = obj->GetRow1();
    const int c1 = obj->GetCol1();
    const int rowTL = r1 - range > 0 ? r1 - range : 0;
    const int colTL = c1 - range > 0 ? c1 - range : 0;
    const int rowBR = r0 + range < rows ? r0 + range : rows - 1;
    const int colBR = c0 + range < cols ? c0 + range : cols - 1;
    const PlayerFaction faction = obj->GetFaction();

    for(int r = rowTL; r <= rowBR; ++r)
    {
        for(int c = colTL; c <= colBR; ++c)
        {
            // skip cells below object
            if(!(r >= r1 && r <= r0 && c >= c1 && c <= c0))
            {
                auto pi = GetNewIndicator(faction);
                mActiveIndicators.emplace_back(pi);

                mLayer->AddObject(pi, r, c);
            }
        }
    }
}

HealingRangeIndicator * OverlayHealRange::GetNewIndicator(PlayerFaction faction)
{
    HealingRangeIndicator * ind = nullptr;

    // create new indicator
    if(mAvailableIndicators.empty())
        ind = new HealingRangeIndicator(faction);
    // reuse existing indicator
    else
    {
        ind = mAvailableIndicators.back();
        mAvailableIndicators.pop_back();

        ind->SetFaction(faction);
    }

    return ind;
}

} // namespace game
