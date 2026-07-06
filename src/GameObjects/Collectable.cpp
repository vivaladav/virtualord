#include "GameObjects/Collectable.h"

#include "Game.h"

#include <sgl/utilities/UniformDistribution.h>

namespace game
{

Collectable::Collectable(const ObjectData & data, const ObjectInitData & initData,
                         unsigned int min, unsigned int max)
    : GameObject(data, initData)
    , mMin(min)
    , mMax(max)
{
    sgl::utilities::UniformDistribution ran(mMin, mMax, GetGame()->GetRandSeed());
    mNum = ran.GetNextValue();
}

void Collectable::RandomizeNumUnits(unsigned int min, unsigned int max)
{
    // clamp values
    if(min < mMin)
        min = mMin;
    if(max > mMax)
        max = mMax;

    // randomize num
    sgl::utilities::UniformDistribution ran(min, max, GetGame()->GetRandSeed());
    mNum = ran.GetNextValue();

    UpdateGraphics();
}

void Collectable::MaximizeUnits()
{
    mNum = mMax;

    UpdateGraphics();
}

void Collectable::MinimizeUnits()
{
    mNum = mMin;

    UpdateGraphics();
}

} // namespace game
