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

protected:
    void UpdateGraphics() override;

private:
    void SetImage();

private:
    int mOutputEnergy = 0;
    int mOutputMaterial = 0;
};

} // namespace game
