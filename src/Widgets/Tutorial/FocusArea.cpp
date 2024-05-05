#include "Widgets/Tutorial/FocusArea.h"

#include "Widgets/GameUIData.h"

#include <sgl/graphic/Camera.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

const float TIME_BLINK_ON = 0.8f;
const float TIME_BLINK_OFF = 0.4f;

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
    mBlinkOn = true;
    mTimerBlinking = TIME_BLINK_ON;
}

void FocusArea::SetScreenArea(int x0, int y0, int w, int h)
{
    auto cam = sgl::graphic::Camera::GetDummyCamera();
    SetCamera(cam);

    SetArea(x0, y0, w, h);
}

void FocusArea::SetWorldArea(int x0, int y0, int w, int h)
{
    auto cam = sgl::graphic::Camera::GetDefaultCamera();
    SetCamera(cam);

    SetArea(x0, y0, w, h);
}

void FocusArea::SetCornersColor(unsigned int color)
{
    mCornerTL->SetColor(color);
    mCornerTR->SetColor(color);
    mCornerBL->SetColor(color);
    mCornerBR->SetColor(color);
}

void FocusArea::SetArea(int x0, int y0, int w, int h)
{
    SetSize(w, h);

    // TOP LEFT
    int x = x0;
    int y = y0;

    mCornerTL->SetPosition(x, y);

    // TOP RIGHT
    x = x0 + w - mCornerTR->GetWidth();
    y = y0;

    mCornerTR->SetPosition(x, y);

    // BOTTOM LEFT
    x = x0;
    y = y0 + h - mCornerBL->GetHeight();

    mCornerBL->SetPosition(x, y);

    // BOTTOM RIGHT
    x = x0 + w - mCornerBR->GetWidth();
    y = y0 + h - mCornerBR->GetHeight();

    mCornerBR->SetPosition(x, y);
}

void FocusArea::OnRender()
{
    // only render if not blinking or in ON state of blinking
    if(!mBlinking || mBlinkOn)
        sgl::sgui::Widget::OnRender();
}

void FocusArea::OnUpdate(float delta)
{
    if(mBlinking && IsVisible())
    {
        mTimerBlinking -= delta;

        if(mTimerBlinking <= 0.f)
        {
            mBlinkOn = !mBlinkOn;

            mTimerBlinking = mBlinkOn ? TIME_BLINK_ON : TIME_BLINK_OFF;
        }
    }
}

} // namespace game
