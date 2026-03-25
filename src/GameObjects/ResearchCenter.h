#pragma once

#include "Structure.h"

#include <vector>

namespace game
{

class ResearchCenter : public Structure
{
public:
    ResearchCenter(const ObjectData & data, const ObjectInitData & initData);
    ~ResearchCenter();

    void OnNewTurn(PlayerFaction faction) override;

    int GetResourceProduction(ExtendedResource res) const override;
    int GetResourceUsage(ExtendedResource res) const override;

    void SetResourceUsage(ExtendedResource res, int val);

private:
    void UpdateGraphics() override;

    void SetImage();

    void UpdateProduction();

private:
    std::vector<int> mResUsage;

    int mResearchPerTurn = 0;

    unsigned int mResTrackerId = 0;
};

} // namespace game
