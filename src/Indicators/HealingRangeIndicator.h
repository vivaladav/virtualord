#pragma once

#include "IsoObject.h"

namespace game
{

enum PlayerFaction : unsigned int;

class HealingRangeIndicator : public IsoObject
{
public:
    HealingRangeIndicator(PlayerFaction faction);

    void SetFaction(PlayerFaction faction);
};

} // namespace game
