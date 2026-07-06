#pragma once

#include "IsoObject.h"

namespace game
{

enum PlayerFaction : unsigned int;

class ConquestIndicator : public IsoObject
{
public:
    ConquestIndicator(PlayerFaction faction);

    void SetDoable(bool doable);

private:
    bool mDoable = true;
};

} // namespace game
