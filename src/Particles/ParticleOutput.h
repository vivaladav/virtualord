#pragma once

#include <sgl/graphic/Particle.h>

namespace sgl
{
    namespace graphic { class TexturedRenderable; }
}

namespace game
{

class GameMap;
class GameObject;

struct DataParticleOutput;

class ParticleOutput : public sgl::graphic::Particle
{
public:
    ParticleOutput();
    ~ParticleOutput();

    void SetData(const DataParticleOutput & data);

    float GetSpeed() const;

    void Update(float delta) override;

    void Render() override;

private:
    void SetStart(int x0, int y0);

private:
    sgl::graphic::TexturedRenderable * mIcon = nullptr;
    sgl::graphic::TexturedRenderable * mTxt = nullptr;

    float mPosXf = 0.f;
    float mPosYf = 0.f;

    float mSpeed = 0.f;
    float mAlpha = 255.f;
    float mDecaySpeed = 1.f;
    float mTimerLife = 1.f;
};

inline float ParticleOutput::GetSpeed() const { return mSpeed; }


} // namespace game
