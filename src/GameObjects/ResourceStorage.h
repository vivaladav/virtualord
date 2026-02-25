#pragma once

#include "Structure.h"

namespace game
{

enum ResourceType : unsigned int;

class ResourceStorage : public Structure
{
public:
    ResourceStorage(const ObjectData & data, const ObjectInitData & initData);

    ResourceType GetResourceType() const;

    void OnCapacityUpgraded();

private:
    void UpdateGraphics() override;

    void OnLinkedChanged() override;
    void OnAttributeChanged() override;

    void UpdateCapacity();
    void UpdateCapacityValue();

    void SetImage();

private:
    ResourceType mResource;

    int mCapacity = 0;
};

inline ResourceType ResourceStorage::GetResourceType() const { return mResource; }

inline void ResourceStorage::OnCapacityUpgraded() { UpdateCapacity(); }

} // namespace game

