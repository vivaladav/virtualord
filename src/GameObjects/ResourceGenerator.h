#pragma once

#include "Structure.h"

namespace game
{

enum ResourceType : unsigned int;

class ResourceGenerator : public Structure
{
public:
    ResourceGenerator(const ObjectData & data, const ObjectInitData & initData);

    void ScaleOutput(float mult);

    ResourceType GetResourceType() const;

    int GetResourceProduction(ExtendedResource res) const override;

    void OnNewTurn(PlayerFaction faction) override;

protected:
    void UpdateGraphics() override;

private:
    void SetImage();

    void UpdateOutput();

private:
    ResourceType mResource;

    int mOutput = 0;
};

inline ResourceType ResourceGenerator::GetResourceType() const { return mResource; }

} // namespace game

