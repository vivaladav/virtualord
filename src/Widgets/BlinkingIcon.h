#pragma once

#include <sgl/sgui/Image.h>

namespace game
{

// ===== BASE BLINKING ICON =====
class BlinkingIcon : public sgl::sgui::Image
{
public:
    BlinkingIcon(const char * file, unsigned int spriteId);

private:
    void OnUpdate(float delta) override;

private:
    float mTimerBlink = 0.f;
};

// ===== ENERGY =====
class BlinkingIconEnergy : public BlinkingIcon
{
public:
    BlinkingIconEnergy();
};

// ===== RESEARCH =====
class BlinkingIconResearch : public BlinkingIcon
{
public:
    BlinkingIconResearch();
};


} // namespace game
