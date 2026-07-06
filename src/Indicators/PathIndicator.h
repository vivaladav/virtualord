#pragma once

#include "IsoObject.h"

namespace game
{

class ObjectData;

enum PlayerFaction : unsigned int;

class PathIndicator : public IsoObject
{
public:
    PathIndicator(PlayerFaction faction, bool final = false);

    void SetFinal(bool final);
    void SetDoable(bool doable);

private:
    void UpdateTexture();

    void UpdateColors();

private:
    PlayerFaction mFaction;

    bool mFinal = false;
    bool mDoable = true;
};

} // namespace game
