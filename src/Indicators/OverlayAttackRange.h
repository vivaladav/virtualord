#pragma once

#include <vector>

namespace game
{

class AttackRangeIndicator;
class GameObject;
class IsoLayer;
class IsoMap;

class OverlayAttackRange
{
public:
    OverlayAttackRange(IsoMap * im);
    ~OverlayAttackRange();

    void Clear();
    void Show(const GameObject * obj);

private:
    AttackRangeIndicator * GetNewIndicator(unsigned int distance, unsigned int range);

private:
    std::vector<AttackRangeIndicator *> mActiveIndicators;
    std::vector<AttackRangeIndicator *> mAvailableIndicators;

    IsoMap * mIsoMap = nullptr;
    IsoLayer * mLayer = nullptr;
};

} // namespace game
