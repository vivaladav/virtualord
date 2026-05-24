#include "Widgets/Tutorial/IsoFocusArea.h"

#include "IsoMap.h"
#include "Tutorial/TutorialConstants.h"
#include "Widgets/GameUIData.h"

#include <sgl/graphic/Image.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

const float TIME_BLINK_ON = 0.8f;
const float TIME_BLINK_OFF = 0.4f;

IsoFocusArea::IsoFocusArea(const IsoMap * im)
    : mIM(im)
{
    using namespace sgl;

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
    mBlinkOn = true;
    mTimerBlinking = TIME_BLINK_ON;
}

void IsoFocusArea::SetCellArea(int r0, int c0, int r1, int c1)
{
    // widget size
    const sgl::core::Pointd2D p0 = mIM->GetCellPosition(r0, c0);
    const sgl::core::Pointd2D p1 = mIM->GetCellPosition(r1, c1);
    const int w = p0.x - p1.x + mIM->GetTileWidth();
    const int h = p0.y - p1.y + mIM->GetTileHeight();

    SetSize(w, h);

    // area size
    const unsigned int rows = r0 - r1 + 1;
    const unsigned int cols = c0 - c1 + 1;

    mCornerTL->SetPosition(p1.x, p1.y);
    mCornerTR->SetPosition(p0.x, p1.y);
    mCornerBR->SetPosition(p0.x, p0.y);
    mCornerBL->SetPosition(p1.x, p0.y);
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
    // only render if not blinking or in ON state of blinking
    if(!mBlinking || mBlinkOn)
        sgl::sgui::Widget::OnRender();
}

void IsoFocusArea::OnUpdate(float delta)
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
