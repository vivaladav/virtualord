#pragma once

#include <sgl/sgui/Image.h>

namespace game
{

class BlinkingIconEnergy : public sgl::sgui::Image
{
public:
    BlinkingIconEnergy();

private:
    void OnUpdate(float delta) override;

private:
    float mTimerBlink = 0.f;
};

} // namespace game
