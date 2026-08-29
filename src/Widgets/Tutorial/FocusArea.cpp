#include "Widgets/Tutorial/FocusArea.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/GameUIData.h"

#include <sgl/graphic/Camera.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/TextureManager.h>

#include <cmath>

namespace
{
constexpr float TIME_BLINK_ON = 0.8f;
constexpr float TIME_BLINK_OFF = 0.4f;

constexpr float GAP_ANIM = 200.f;
}

namespace game
{

FocusArea::FocusArea()
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();

    mCornerTL = new graphic::Image(tm->GetSprite(SpriteFileTutorial, IND_TUT_AREA_CORNER_TL));
    RegisterRenderable(mCornerTL);

    mCornerTR = new graphic::Image(tm->GetSprite(SpriteFileTutorial, IND_TUT_AREA_CORNER_TR));
    RegisterRenderable(mCornerTR);

    mCornerBL = new graphic::Image(tm->GetSprite(SpriteFileTutorial, IND_TUT_AREA_CORNER_BL));
    RegisterRenderable(mCornerBL);

    mCornerBR = new graphic::Image(tm->GetSprite(SpriteFileTutorial, IND_TUT_AREA_CORNER_BR));
    RegisterRenderable(mCornerBR);
}

void FocusArea::SetBlinking(bool enabled)
{
    if(enabled == mBlinking)
        return ;

    mBlinking = enabled;

    // reset blinking state
    mRendering = !mAnimating;
    mTimerBlinking = TIME_BLINK_ON;
}

void FocusArea::SetScreenArea(int x0, int y0, int w, int h, bool anim, float delayAnim)
{
    auto cam = sgl::graphic::Camera::GetDummyCamera();
    SetCamera(cam);

    SetArea(x0, y0, w, h, anim, delayAnim);
}

void FocusArea::SetWorldArea(int x0, int y0, int w, int h, bool anim, float delayAnim)
{
    auto cam = sgl::graphic::Camera::GetDefaultCamera();
    SetCamera(cam);

    SetArea(x0, y0, w, h, anim, delayAnim);
}

void FocusArea::SetCornersColor(unsigned int color)
{
    mCornerTL->SetColor(color);
    mCornerTR->SetColor(color);
    mCornerBL->SetColor(color);
    mCornerBR->SetColor(color);
}

void FocusArea::SetCornersColorElement()
{
    SetCornersColor(TutorialConstants::colorFocusElement);
}

void FocusArea::SetCornersColorAction()
{
    SetCornersColor(TutorialConstants::colorFocusAction);
}

void FocusArea::SetArea(int x0, int y0, int w, int h, bool anim, float delayAnim)
{
    SetSize(w, h);

    const float padding = GAP_ANIM * anim;

    // TOP LEFT
    mTargetCornerPosTL.x = x0;
    mTargetCornerPosTL.y = y0;
    mCornerPosTL.x = mTargetCornerPosTL.x - padding;
    mCornerPosTL.y = mTargetCornerPosTL.y - padding;
    mCornerTL->SetPosition(mCornerPosTL.x, mCornerPosTL.y);

    // TOP RIGHT
    mTargetCornerPosTR.x = x0 + w - mCornerTR->GetWidth();
    mTargetCornerPosTR.y = y0;
    mCornerPosTR.x = mTargetCornerPosTR.x + padding;
    mCornerPosTR.y = mTargetCornerPosTR.y - padding;
    mCornerTR->SetPosition(mCornerPosTR.x, mCornerPosTR.y);

    // BOTTOM LEFT
    mTargetCornerPosBL.x = x0;
    mTargetCornerPosBL.y = y0 + h - mCornerBL->GetHeight();
    mCornerPosBL.x = mTargetCornerPosBL.x - padding;
    mCornerPosBL.y = mTargetCornerPosBL.y + padding;
    mCornerBL->SetPosition(mCornerPosBL.x, mCornerPosBL.y);

    // BOTTOM RIGHT
    mTargetCornerPosBR.x = x0 + w - mCornerBR->GetWidth();
    mTargetCornerPosBR.y = y0 + h - mCornerBR->GetHeight();
    mCornerPosBR.x = mTargetCornerPosBR.x + padding;
    mCornerPosBR.y = mTargetCornerPosBR.y + padding;
    mCornerBR->SetPosition(mCornerPosBR.x, mCornerPosBR.y);

    mRendering = !anim;
    mAnimating = anim;
    mAnimationDelay = delayAnim;
    mAnimationMove = 0.f;
}

void FocusArea::OnRender()
{
    // control rendering for animation delay and blinking
    if(mRendering)
        sgl::sgui::Widget::OnRender();
}

void FocusArea::OnUpdate(float delta)
{
    // do nothing while not visible
    if(!IsVisible())
        return;

    if(mAnimating)
    {
        if(mAnimationDelay > 0.f)
        {
            mAnimationDelay -= delta;

            if(mAnimationDelay < 0.f)
                mRendering = true;
            else
                return;
        }

        const float animSpeed = 500.f;

        const float move = delta * animSpeed;
        mAnimationMove += move;

        mCornerPosTL.x += move;
        mCornerPosTL.y += move;
        mCornerTL->SetPosition(std::roundf(mCornerPosTL.x), std::roundf(mCornerPosTL.y));

        mCornerPosTR.x -= move;
        mCornerPosTR.y += move;
        mCornerTR->SetPosition(std::roundf(mCornerPosTR.x), std::roundf(mCornerPosTR.y));

        mCornerPosBL.x += move;
        mCornerPosBL.y -= move;
        mCornerBL->SetPosition(std::roundf(mCornerPosBL.x), std::roundf(mCornerPosBL.y));

        mCornerPosBR.x -= move;
        mCornerPosBR.y -= move;
        mCornerBR->SetPosition(std::roundf(mCornerPosBR.x), std::roundf(mCornerPosBR.y));

        mAnimating = mAnimationMove < GAP_ANIM;

        // finished animation -> make sure positions are correct
        if(!mAnimating)
        {
            mCornerTL->SetPosition(mTargetCornerPosTL.x, mTargetCornerPosTL.y);
            mCornerTR->SetPosition(mTargetCornerPosTR.x, mTargetCornerPosTR.y);
            mCornerBL->SetPosition(mTargetCornerPosBL.x, mTargetCornerPosBL.y);
            mCornerBR->SetPosition(mTargetCornerPosBR.x, mTargetCornerPosBR.y);
        }
    }
    else if(mBlinking && IsVisible())
    {
        mTimerBlinking -= delta;

        if(mTimerBlinking <= 0.f)
        {
            mRendering = !mRendering;

            mTimerBlinking = mRendering ? TIME_BLINK_ON : TIME_BLINK_OFF;
        }
    }
}

} // namespace game
