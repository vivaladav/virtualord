#pragma once

#include <sgl/graphic/ParticleData.h>

namespace game
{

enum OutputType : unsigned int
{
    OT_BLOBS,
    OT_DIAMONDS,
    OT_ENERGY,
    OT_MATERIAL,
    OT_MONEY,
    OT_RESEARCH,

    NUM_OUTPUT_TYPES,

    OT_NULL
};

struct DataParticleOutput : public sgl::graphic::ParticleData
{
    DataParticleOutput(int q, OutputType t, float x, float y)
        : sgl::graphic::ParticleData(x, y, 40.f)
        , quantity(q)
        , type(t)
    {
    }

    DataParticleOutput(int q, OutputType t, float x, float y, float sp, float ds, float tl)
        : sgl::graphic::ParticleData(x, y, sp)
        , quantity(q)
        , type(t)
        , decaySpeed(ds)
        , timeLife(tl)
    {
    }

    int quantity = 0;
    OutputType type;
    float decaySpeed = 100.f;
    float timeLife = 1.f;
};

} // namespace game
