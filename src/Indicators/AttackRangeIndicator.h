#pragma once

#include "IsoObject.h"

namespace game
{

enum PlayerFaction : unsigned int;

class AttackRangeIndicator : public IsoObject
{
public:
    AttackRangeIndicator(unsigned int distance, unsigned int range);

    void SetDistance(unsigned int distance, unsigned int range);

private:
    unsigned int mDistance = 0;
    unsigned int mRange = 0;
};

} // namespace game
