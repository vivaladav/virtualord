#include "Indicators/OverlayStructure.h"

#include "IsoLayer.h"
#include "IsoMap.h"
#include "GameObjects/ObjectsDataRegistry.h"
#include "Indicators/StructureIndicator.h"
#include "Widgets/PanelUnitEnergyUsage.h"

namespace game
{

OverlayStructure::OverlayStructure(IsoMap * im, const ObjectsDataRegistry * reg,
                                   PlayerFaction faction)
    : mObjDataReg(reg)
    , mPanelUnitCost(new PanelUnitEnergyUsage)
    , mIsoMap(im)
    , mLayer(im->GetLayer(MapLayers::CELL_OVERLAYS2))
    , mFaction(faction)
{
    mPanelUnitCost->SetVisible(false);
}

OverlayStructure::~OverlayStructure()
{
    for(auto it : mIndicators)
        delete it.second;
}

void OverlayStructure::ClearIndicator()
{
    if(mIndicator != nullptr)
    {
        mLayer->RemoveObject(mIndicator);

        mIndicator = nullptr;
    }

    mPanelUnitCost->SetVisible(false);
}

void OverlayStructure::ShowIndicator(GameObjectTypeId type, int row, int col,
                                     int unitCost, bool doable)
{
    auto ind = GetNewIndicator(type);

    // remove previous indicator if not the right type
    if(mIndicator != ind)
    {
        ClearIndicator();
        mIndicator = ind;
    }

    // moving the same indicator
    if(mLayer->HasObject(mIndicator))
        mLayer->MoveObject(mIndicator, row, col);
    // indicator not visible yet
    else
        mLayer->AddObject(mIndicator, row, col);

    if(unitCost > 0)
    {
        mPanelUnitCost->SetValue(unitCost);
        mPanelUnitCost->SetDoable(doable);
        mPanelUnitCost->SetVisible(true);

        const int x = mIndicator->GetX() + (mIndicator->GetWidth() - mPanelUnitCost->GetWidth()) / 2;
        const int y = mIndicator->GetY() - mPanelUnitCost->GetHeight();
        mPanelUnitCost->SetPosition(x, y);
    }
    else
        mPanelUnitCost->SetVisible(false);
}

StructureIndicator * OverlayStructure::GetNewIndicator(GameObjectTypeId type)
{
    StructureIndicator * ind = nullptr;

    auto it = mIndicators.find(type);

    if(it != mIndicators.end())
        ind = it->second;
    else
    {
        ind = new StructureIndicator(mObjDataReg->GetObjectData(type), mFaction);
        mIndicators.emplace(type, ind);
    }

    return ind;
}

} // namespace game