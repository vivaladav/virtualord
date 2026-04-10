#pragma once

#include <vector>

namespace game
{

class HealingRangeIndicator;
class GameObject;
class IsoLayer;
class IsoMap;

enum PlayerFaction : unsigned int;

class OverlayHealRange
{
public:
    OverlayHealRange(IsoMap * im);
    ~OverlayHealRange();

    void Clear();
    void Show(const GameObject * obj, unsigned int range);

private:
    HealingRangeIndicator * GetNewIndicator(PlayerFaction faction);

private:
    std::vector<HealingRangeIndicator *> mActiveIndicators;
    std::vector<HealingRangeIndicator *> mAvailableIndicators;

    IsoMap * mIsoMap = nullptr;
    IsoLayer * mLayer = nullptr;
};

} // namespace game
