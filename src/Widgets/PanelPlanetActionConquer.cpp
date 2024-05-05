#include "Widgets/PanelPlanetActionConquer.h"

#include "Widgets/ButtonPlanetMap.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>
#include <sgl/sgui/TextArea.h>

namespace
{
    constexpr unsigned int textColor = 0x80a2b3ff;
    constexpr int textSize = 18;
    const char * fileFont = "Lato-Regular.ttf";
}

namespace game
{

PanelPlanetActionConquer::PanelPlanetActionConquer()
    : sgl::sgui::Widget(nullptr)
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();
    auto fm = graphic::FontManager::Instance();

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFilePlanetMap2, IND_PM_PANEL_ACTIONS);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    SetSize(tex->GetWidth(), tex->GetHeight());

    // TITLE
    const unsigned int colorTitle = 0xe9f7fbcc;

    graphic::Font * fnt = fm->GetFont(fileFont, WidgetsConstants::FontSizePlanetMapTitle,
                                      graphic::Font::NORMAL);
    mTitle = new graphic::Text("CONQUER", fnt);
    mTitle->SetColor(colorTitle);
    RegisterRenderable(mTitle);

    // CONTENT
    CreateContentStart();

    // BUTTONS
    mButtonOk = new ButtonPlanetMap(this);
    mButtonOk->SetLabel("PROCEED");

    mButtonCancel = new SecondaryButtonPlanetMap(this);
    mButtonCancel->SetLabel("CANCEL");

    // position elements
    UpdatePositions();
}

void PanelPlanetActionConquer::AddOnButtonOkClickFunction(const std::function<void()> & f)
{
    mButtonOk->AddOnClickFunction(f);
}

void PanelPlanetActionConquer::AddOnButtonCancelClickFunction(const std::function<void()> & f)
{
    mButtonCancel->AddOnClickFunction(f);
}

sgl::sgui::AbstractButton * PanelPlanetActionConquer::GetButtonOk() const
{
    return mButtonOk;
}

sgl::sgui::AbstractButton * PanelPlanetActionConquer::GetButtonCancel() const
{
    return mButtonCancel;
}

void PanelPlanetActionConquer::CreateContentStart()
{
    using namespace sgl;

    mContentStart = new sgui::Widget(this);

    auto fm = graphic::FontManager::Instance();

    const int w = GetWidth();

    // DESCRIPTION
    graphic::Font * fnt = fm->GetFont(fileFont, textSize, graphic::Font::NORMAL);

    const int marginL = 20;
    const int marginR = 20;
    const int contW = w - marginL - marginR;
    const int contH = 100;
    const char * txt = "Start a mission to conquer the territory.\n\n"
                       "You will personally lead the construction of a new base and your troops in battle.";
    auto text = new sgui::TextArea(contW, contH, txt, fnt, false, mContentStart);
    text->SetColor(textColor);
}

void PanelPlanetActionConquer::HandlePositionChanged()
{
    UpdatePositions();
}

void PanelPlanetActionConquer::UpdatePositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();
    const int w = GetWidth();
    const int h = GetHeight();

    const int marginL = 20;
    const int marginT = 15;
    const int marginTextT = 5;

    int x;
    int y;

    // BACKGROUND
    mBg->SetPosition(x0, y0);

    // TITLE
    x = x0 + marginL;
    y = y0 + marginT;

    mTitle->SetPosition(x, y);

    // CONTENT
    x = marginL;
    y = marginT + mTitle->GetHeight() + marginTextT;

    mContentStart->SetPosition(x, y);

    // BUTTONS
    const int marginButtonsBottom = 50;
    const int spacingButtons = 20;

    x = (w - mButtonCancel->GetWidth()) * 0.5f;
    y = h - marginButtonsBottom - mButtonCancel->GetHeight();

    mButtonCancel->SetPosition(x, y);

    y -= spacingButtons + mButtonOk->GetHeight();

    mButtonOk->SetPosition(x, y);
}

} // namespace game
