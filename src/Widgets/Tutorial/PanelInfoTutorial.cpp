#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include "Widgets/GameUIData.h"

#include <sgl/graphic/Image.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>

namespace game
{

PanelInfoTutorial::PanelInfoTutorial(int w, int h)
{
    using namespace sgl;

    SetSize(w, h);

    // CREATE BACKGROUND
    auto tm = graphic::TextureManager::Instance();
    graphic::Texture * tex = nullptr;

    const unsigned int texIds[NUM_BGPARTS] =
    {
        IND_TUT_PANEL_INFO_CORNER_TL,
        IND_TUT_PANEL_INFO_CORNER_TR,
        IND_TUT_PANEL_INFO_CORNER_BL,
        IND_TUT_PANEL_INFO_CORNER_BR,
        IND_TUT_PANEL_INFO_BG,
        IND_TUT_PANEL_INFO_L,
        IND_TUT_PANEL_INFO_R,
        IND_TUT_PANEL_INFO_T,
        IND_TUT_PANEL_INFO_B
    };

    for(unsigned int ind = BGPART_TL; ind < NUM_BGPARTS; ++ind)
    {
        tex = tm->GetSprite(SpriteFileTutorialExp, texIds[ind]);
        tex->SetScaleMode(0);

        mBgParts[ind] = new graphic::Image(tex);
        RegisterRenderable(mBgParts[ind]);
    }

    PositionElements();
}

void PanelInfoTutorial::HandlePositionChanged()
{
    PositionElements();
}

void PanelInfoTutorial::PositionElements()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();
    const int w = GetWidth();
    const int h = GetHeight();
    const int cornerW = mBgParts[BGPART_TL]->GetWidth();
    const int cornerH = mBgParts[BGPART_TL]->GetHeight();

    // -- BACKGROUND --
    int x = x0;
    int y = y0;
    int pW, pH;

    // TL
    mBgParts[BGPART_TL]->SetPosition(x, y);

    // T
    x += cornerW;
    pW = w - (2 * cornerW);

    mBgParts[BGPART_T]->SetPosition(x, y);
    mBgParts[BGPART_T]->SetWidth(pW);

    // TR
    x += pW;

    mBgParts[BGPART_TR]->SetPosition(x, y);

    // L
    x = x0;
    y += cornerH;
    pH = h - (2 * cornerH);

    mBgParts[BGPART_L]->SetPosition(x, y);
    mBgParts[BGPART_L]->SetHeight(pH);

    // CENTER
    x += cornerW;
    pW = w - (2 * cornerW);
    pH = h - (2 * cornerH);

    mBgParts[BGPART_CENTER]->SetPosition(x, y);
    mBgParts[BGPART_CENTER]->SetWidth(pW);
    mBgParts[BGPART_CENTER]->SetHeight(pH);

    // R
    x += pW;
    pH = h - (2 * cornerH);

    mBgParts[BGPART_R]->SetPosition(x, y);
    mBgParts[BGPART_R]->SetHeight(pH);

    // BL
    x = x0;
    y += pH;

    mBgParts[BGPART_BL]->SetPosition(x, y);

    // B
    x += cornerW;
    pW = w - (2 * cornerW);

    mBgParts[BGPART_B]->SetPosition(x, y);
    mBgParts[BGPART_B]->SetWidth(pW);

    // BR
    x += pW;

    mBgParts[BGPART_BR]->SetPosition(x, y);
}

} // namespace game
