#pragma once

#include "Structure.h"

namespace game
{

enum ExtendedResource : unsigned int;

class ResourceGenerator : public Structure
{
public:
    ResourceGenerator(const ObjectData & data, const ObjectInitData & initData);

    bool Load(sgl::utilities::BinaryFile & bf) override;
    bool Save(sgl::utilities::BinaryFile & bf) const override;

    void ScaleOutput(float mult);

    ExtendedResource GetExtendedResource() const;

    int GetResourceProduction(ExtendedResource res) const override;

    void OnTurnStart(PlayerFaction faction) override;

private:
    void UpdateGraphics() override;

    void SetImage();

    void UpdateOutput();

private:
    ExtendedResource mResource;

    int mOutput = 0;
};

inline ExtendedResource ResourceGenerator::GetExtendedResource() const { return mResource; }

} // namespace game

