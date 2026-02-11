#pragma once

#include "Structure.h"

namespace game
{

class Base : public Structure
{
public:
    Base(const ObjectData & data, const ObjectInitData & initData);

    void OnNewTurn(PlayerFaction faction) override;

    int GetResourceProduction(ExtendedResource res) const override;

private:
    void UpdateGraphics() override;

    void SetImage();

private:
    int mOutputEnergy = 0;
    int mOutputMaterial = 0;
};

} // namespace game
