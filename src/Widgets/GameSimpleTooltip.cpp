#include "GameSimpleTooltip.h"

#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/GraphicConstants.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/Texture.h>

// anonymous namespace for private stuff
namespace
{
constexpr int marginL = 30;
}

namespace game
{

GameSimpleTooltip::GameSimpleTooltip(const char * text)
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFileTooltips, ID_TOOLTIP_GAME_BG_L);

    mBgL = new graphic::Image(tex);
    RegisterRenderable(mBgL);

    tex = tm->GetSprite(SpriteFileTooltips, ID_TOOLTIP_GAME_BG_R);
    mBgR = new graphic::Image(tex);
    RegisterRenderable(mBgR);

    tex = tm->GetSprite(SpriteFileTooltipsExp, ID_TOOLTIP_GAME_BG_C);
    tex->SetScaleMode(graphic::TSCALE_NEAREST);
    mBgC = new graphic::Image(tex);
    RegisterRenderable(mBgC);

    // LABEL
    SetText(text);
}

void GameSimpleTooltip::SetText(const char * text)
{
    using namespace sgl;

    if(mLabel != nullptr)
    {
        UnregisterRenderable(mLabel);
        delete mLabel;
    }

    auto fm = graphic::FontManager::Instance();
    auto font = fm->GetFont(WidgetsConstants::FontFileText, 16, graphic::Font::NORMAL);

    mLabel = new graphic::Text(text, font);
    mLabel->SetColor(WidgetsConstants::colorTooltipText);
    RegisterRenderable(mLabel);

    // SET SIZES
    const int contentW = mLabel->GetWidth() - (mBgL->GetWidth() - marginL);
    const int w = mBgL->GetWidth() + contentW + mBgR->GetWidth();
    const int h = mBgC->GetHeight();

    mBgC->SetWidth(contentW);

    SetSize(w, h);

    // reposition
    SetPositions();
}

void GameSimpleTooltip::HandlePositionChanged()
{
    SetPositions();
}

void GameSimpleTooltip::SetPositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    // BACKGROUND
    int x = x0;
    int y = y0;

    mBgL->SetPosition(x, y);
    x += mBgL->GetWidth();

    mBgC->SetPosition(x, y);
    x += mBgC->GetWidth();

    mBgR->SetPosition(x, y);

    // LABEL
    const int labelX = x0 + marginL;
    const int labelY = y0 + (mBgC->GetHeight() - mLabel->GetHeight()) / 2;

    mLabel->SetPosition(labelX, labelY);
}

} // namespace game
