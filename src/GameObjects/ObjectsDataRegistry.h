#pragma once

#include "GameObjects/GameObjectTypes.h"
#include "ObjectData.h"

#include <unordered_map>

namespace game
{

class ObjectsDataRegistry
{
public:
    ObjectsDataRegistry();

    const ObjectData & GetObjectData(GameObjectTypeId type) const;

private:
    void InitObjectData();

private:
    std::unordered_map<GameObjectTypeId, ObjectData> mData;
};

} // namespace game
