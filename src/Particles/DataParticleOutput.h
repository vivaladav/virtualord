#pragma once

#include <sgl/graphic/ParticleData.h>

namespace game
{

enum ExtendedResource : unsigned int;

struct DataParticleOutput : public sgl::graphic::ParticleData
{
    DataParticleOutput(int q, ExtendedResource t, float x, float y, float de)
        : sgl::graphic::ParticleData(x, y, 30.f, de)
        , quantity(q)
        , type(t)
    {
    }

    DataParticleOutput(int q, ExtendedResource t, float x, float y, float sp, float de,
                       float ds, float tl)
        : sgl::graphic::ParticleData(x, y, sp, de)
        , quantity(q)
        , type(t)
        , decaySpeed(ds)
        , timeLife(tl)
    {
    }

    int quantity = 0;
    ExtendedResource type;
    float decaySpeed = 0.f;
    float timeLife = 1.5f;
};

} // namespace game
