#include "Widgets/Tutorial/FocusArea.h"

#include "Widgets/GameUIData.h"

#include <sgl/graphic/Camera.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/TextureManager.h>

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

} // namespace game
