#pragma once

#include "GameObjects/GameObjectTypes.h"

#include <unordered_map>

namespace game
{

class IsoLayer;
class IsoMap;
class ObjectsDataRegistry;
class StructureIndicator;

enum PlayerFaction : unsigned int;

class OverlayStructure
{
public:
    OverlayStructure(IsoMap * im, const ObjectsDataRegistry * reg, PlayerFaction faction);
    ~OverlayStructure();

    void ClearIndicator();
    void ShowIndicator(GameObjectTypeId type, int row, int col);

private:
    StructureIndicator * GetNewIndicator(GameObjectTypeId type);

private:
    std::unordered_map<GameObjectTypeId, StructureIndicator *> mIndicators;

    StructureIndicator * mIndicator = nullptr;

    const ObjectsDataRegistry * mObjDataReg = nullptr;

    IsoMap * mIsoMap = nullptr;
    IsoLayer * mLayer = nullptr;

    PlayerFaction mFaction;
};

} // namespace game
