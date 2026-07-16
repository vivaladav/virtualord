#include "Widgets/Tutorial/IsoFocusArea.h"

#include "IsoMap.h"
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

constexpr float GAP_ANIM_X = 200.f;
constexpr float GAP_ANIM_Y = 100.f;
}

namespace game
{

IsoFocusArea::IsoFocusArea(const IsoMap * im)
    : mIM(im)
{
    using namespace sgl;

    SetCamera(sgl::graphic::Camera::GetDefaultCamera());

    auto tm = graphic::TextureManager::Instance();

    mCornerTL = new graphic::Image(tm->GetSprite(SpriteFileTutorial, IND_TUT_PANEL_INFO_ISO_CORNER_TL));
    RegisterRenderable(mCornerTL);

    mCornerTR = new graphic::Image(tm->GetSprite(SpriteFileTutorial, IND_TUT_PANEL_INFO_ISO_CORNER_TR));
    RegisterRenderable(mCornerTR);

    mCornerBL = new graphic::Image(tm->GetSprite(SpriteFileTutorial, IND_TUT_PANEL_INFO_ISO_CORNER_BL));
    RegisterRenderable(mCornerBL);

    mCornerBR = new graphic::Image(tm->GetSprite(SpriteFileTutorial, IND_TUT_PANEL_INFO_ISO_CORNER_BR));
    RegisterRenderable(mCornerBR);
}

void IsoFocusArea::SetBlinking(bool enabled)
{
    if(enabled == mBlinking)
        return ;

    mBlinking = enabled;

    // reset blinking state
    mRendering = !mAnimating;
    mTimerBlinking = TIME_BLINK_ON;
}

void IsoFocusArea::SetCellArea(int r0, int c0, int r1, int c1, bool anim, float delayAnim)
{
    const float paddingX = GAP_ANIM_X * anim;
    const float paddingY = GAP_ANIM_Y * anim;

    // widget size
    const sgl::core::Pointd2D p0 = mIM->GetCellPosition(r0, c0);
    const sgl::core::Pointd2D p1 = mIM->GetCellPosition(r1, c1);
    const int w = p0.x - p1.x + mIM->GetTileWidth();
    const int h = p0.y - p1.y + mIM->GetTileHeight();

    SetSize(w, h);

    // position corners
    const sgl::core::Pointd2D pTR = mIM->GetCellPosition(r1, c0);
    const sgl::core::Pointd2D pBL = mIM->GetCellPosition(r0, c1);

    // TOP LEFT
    mCornerPosTL.x = p1.x;
    mCornerPosTL.y = p1.y - paddingY;
    mCornerTL->SetPosition(mCornerPosTL.x, mCornerPosTL.y);

    // TOP RIGHT
    mCornerPosTR.x = pTR.x + paddingX;
    mCornerPosTR.y = pTR.y;
    mCornerTR->SetPosition(mCornerPosTR.x, mCornerPosTR.y);

    // BOTTOM LEFT
    mCornerPosBL.x = pBL.x - paddingX;
    mCornerPosBL.y = pBL.y;
    mCornerBL->SetPosition(mCornerPosBL.x, mCornerPosBL.y);

    // BOTTOM RIGHT
    mCornerPosBR.x = p0.x;
    mCornerPosBR.y = p0.y + paddingY;
    mCornerBR->SetPosition(mCornerPosBR.x, mCornerPosBR.y);

    mRendering = !anim;
    mAnimating = anim;
    mAnimationDelay = delayAnim;
}

void IsoFocusArea::SetCornersColor(unsigned int color)
{
    mCornerTL->SetColor(color);
    mCornerTR->SetColor(color);
    mCornerBL->SetColor(color);
    mCornerBR->SetColor(color);
}

void IsoFocusArea::SetCornersColorElement()
{
    SetCornersColor(TutorialConstants::colorFocusElement);
}

void IsoFocusArea::SetCornersColorAction()
{
    SetCornersColor(TutorialConstants::colorFocusAction);
}

void IsoFocusArea::OnRender()
{
    // control rendering for animation delay and blinking
    if(mRendering)
        sgl::sgui::Widget::OnRender();
}

void IsoFocusArea::OnUpdate(float delta)
{
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

        const float moveX = delta * animSpeed;
        const float moveY = moveX * 0.5f;
        mAnimationMove += moveX;

        mCornerPosTL.y += moveY;
        mCornerTL->SetPosition(std::roundf(mCornerPosTL.x), std::roundf(mCornerPosTL.y));

        mCornerPosTR.x -= moveX;
        mCornerTR->SetPosition(std::roundf(mCornerPosTR.x), std::roundf(mCornerPosTR.y));

        mCornerPosBL.x += moveX;
        mCornerBL->SetPosition(std::roundf(mCornerPosBL.x), std::roundf(mCornerPosBL.y));

        mCornerPosBR.y -= moveY;
        mCornerBR->SetPosition(std::roundf(mCornerPosBR.x), std::roundf(mCornerPosBR.y));

        mAnimating = mAnimationMove < GAP_ANIM_X;
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
