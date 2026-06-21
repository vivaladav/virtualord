#pragma once

#include <sgl/sgui/Image.h>

namespace game
{

class BlinkingHighlight : public sgl::sgui::Image
{
public:
    BlinkingHighlight(const char * file, unsigned int spriteId, float maxAlpha = 77.f);

    void SetMaxAlpha(float val);
    void SetSpeed(float val);

private:
    void OnUpdate(float delta) override;

    void HandleStateEnabled() override;

private:
    float mAlphaAnim = 0.f;
    int mAlphaAnimD = 0;
    float mMaxAlpha = 255.f;
    float mSpeedAlpha = 150.f;
};

inline void BlinkingHighlight::SetMaxAlpha(float val) { mMaxAlpha = val; }

inline void BlinkingHighlight::SetSpeed(float val) { mSpeedAlpha = val; }

} // namespace game
