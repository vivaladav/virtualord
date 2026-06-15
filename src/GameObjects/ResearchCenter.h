#pragma once

#include "Structure.h"

#include <vector>

namespace game
{

class BlinkingHighlight;
class BlinkingIconResearch;

class ResearchCenter : public Structure
{
public:
    ResearchCenter(const ObjectData & data, const ObjectInitData & initData);
    ~ResearchCenter();

    void OnNewTurn(PlayerFaction faction) override;

    int GetResourceProduction(ExtendedResource res) const override;
    int GetResourceUsage(ExtendedResource res) const override;

    void SetResourceUsage(ExtendedResource res, int val);

    void OnPositionChanged() override;

private:
    void UpdateGraphics() override;

    void SetImage();

    void UpdateProduction();

    void HideIconResearch();
    void ShowIconResearch();
    void PositionIconResearch();
    void UpdateIconResearch();

    void HideHighlight();
    void ShowHighlight();

private:
    std::vector<int> mResUsage;

    BlinkingHighlight * mHighlight = nullptr;
    BlinkingIconResearch * mIconResearch = nullptr;

    int mResearchPerTurn = 0;

    unsigned int mResTrackerId = 0;

    float mAlphaAnim = 0.f;
};

} // namespace game
