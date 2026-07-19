#include "Indicators/OverlayAttackRange.h"

#include "GameConstants.h"
#include "IsoLayer.h"
#include "IsoMap.h"
#include "GameObjects/GameObject.h"
#include "GameObjectTools/Weapon.h"
#include "Indicators/AttackRangeIndicator.h"

#include <cassert>

namespace game
{

OverlayAttackRange::OverlayAttackRange(IsoMap * im)
    : mIsoMap(im)
    , mLayer(im->GetLayer(MapLayers::CELL_OVERLAYS0))
{
}

OverlayAttackRange::~OverlayAttackRange()
{
    for(auto pi : mActiveIndicators)
        delete pi;

    for(auto pi : mAvailableIndicators)
        delete pi;
}

void OverlayAttackRange::Clear()
{
    for(auto pi : mActiveIndicators)
    {
        mLayer->RemoveObject(pi);

        mAvailableIndicators.emplace_back(pi);
    }

    mActiveIndicators.clear();
}

void OverlayAttackRange::Show(const GameObject * obj)
{
    // clear current indicators
    Clear();

    // show indicators around object
    const int rows = mIsoMap->GetNumRows();
    const int cols = mIsoMap->GetNumCols();
    const int r0 = obj->GetRow0();
    const int c0 = obj->GetCol0();
    const int range = obj->GetWeapon()->GetRange();
    const int rowTL = obj->GetRow1() - range > 0 ? obj->GetRow1() - range : 0;
    const int colTL = obj->GetCol1() - range > 0 ? obj->GetCol1() - range : 0;
    const int rowBR = r0 + range < rows ? r0 + range : rows - 1;
    const int colBR = c0 + range < cols ? c0 + range : cols - 1;

    for(int r = rowTL; r <= rowBR; ++r)
    {
        for(int c = colTL; c <= colBR; ++c)
        {
            // skip cell below object
            if(r != r0 || c != c0)
            {
                const int distR = std::abs(r - r0);
                const int distC = std::abs(c - c0);
                const int dist = distR > distC ? distR : distC;

                auto pi = GetNewIndicator(dist, range);
                mActiveIndicators.emplace_back(pi);

                mLayer->AddObject(pi, r, c);
            }
        }
    }
}

AttackRangeIndicator * OverlayAttackRange::GetNewIndicator(unsigned int distance, unsigned int range)
{
    AttackRangeIndicator * ind = nullptr;

    // create new indicator
    if(mAvailableIndicators.empty())
        ind = new AttackRangeIndicator(distance, range);
    // reuse existing indicator
    else
    {
        ind = mAvailableIndicators.back();
        mAvailableIndicators.pop_back();

        ind->SetDistance(distance, range);
    }

    return ind;
}

} // namespace game
