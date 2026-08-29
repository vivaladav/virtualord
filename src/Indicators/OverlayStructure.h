#pragma once

#include "GameObjects/GameObjectTypes.h"

#include <unordered_map>

namespace game
{

class IsoLayer;
class IsoMap;
class ObjectsDataRegistry;
class PanelUnitEnergyUsage;
class StructureIndicator;

enum PlayerFaction : unsigned int;

class OverlayStructure
{
public:
    OverlayStructure(IsoLayer * layer, const ObjectsDataRegistry * reg, PlayerFaction faction);
    ~OverlayStructure();

    void ClearIndicator();
    void ShowIndicator(GameObjectTypeId type, int row, int col,
                       int unitCost = -1, bool doable = true);

private:
    StructureIndicator * GetNewIndicator(GameObjectTypeId type, bool doable);

private:
    std::unordered_map<GameObjectTypeId, StructureIndicator *> mIndicators;

    StructureIndicator * mIndicator = nullptr;

    const ObjectsDataRegistry * mObjDataReg = nullptr;

    PanelUnitEnergyUsage * mPanelUnitCost = nullptr;

    IsoLayer * mLayer = nullptr;

    PlayerFaction mFaction;
};

} // namespace game
