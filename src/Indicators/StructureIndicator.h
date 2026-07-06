#pragma once

#include "IsoObject.h"

namespace game
{

class ObjectData;

enum PlayerFaction : unsigned int;

class StructureIndicator : public IsoObject
{
public:
    StructureIndicator(const ObjectData & objData, PlayerFaction faction);
};

} // namespace game
