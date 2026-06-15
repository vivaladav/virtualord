#include "BlinkingHighlight.h"

#include <sgl/graphic/Camera.h>
#include <sgl/graphic/TextureManager.h>

#include <cmath>

namespace game
{

BlinkingHighlight::BlinkingHighlight(const char * file, unsigned int spriteId, float maxAlpha)
    : mMaxAlpha(maxAlpha)
{
    using namespace sgl;

    // BlinkingHighlight is part of the game scene
    SetCamera(graphic::Camera::GetDefaultCamera());

    auto tm = graphic::TextureManager::Instance();
    auto tex = tm->GetSprite(file, spriteId);
    SetTexture(tex);

    SetAlpha(0);
}

void BlinkingHighlight::OnUpdate(float delta)
{
    mAlphaAnim += delta * mSpeedAlpha;

    // invert animation direction when reached limit
    if(mSpeedAlpha > 0.f)
    {
        if(mAlphaAnim > mMaxAlpha)
        {
            mAlphaAnim = mMaxAlpha;
            mSpeedAlpha = -mSpeedAlpha;
        }
    }
    else
    {
        if(mAlphaAnim < 0.f)
        {
            mAlphaAnim = 0.f;
            mSpeedAlpha = -mSpeedAlpha;
        }
    }

    // update alpha value if changed
    int a = std::roundf(mAlphaAnim);

    if(a != mAlphaAnimD)
    {
        mAlphaAnimD = a;
        SetAlpha(mAlphaAnimD);
    }
}

} // namespace game
