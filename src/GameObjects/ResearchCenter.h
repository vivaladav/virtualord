#pragma once

#include "Structure.h"

#include <vector>

namespace game
{

class ResearchCenter : public Structure
{
public:
    ResearchCenter(const ObjectData & data, const ObjectInitData & initData);

    void OnNewTurn(PlayerFaction faction) override;

    int GetResourceProduction(ExtendedResource res) const override;
    int GetResourceUsage(ExtendedResource res) const override;

private:
    void UpdateGraphics() override;

    void SetImage();

private:
    std::vector<int> mResUsage;

    int mResearchPerTurn = 50;
};

} // namespace game
