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
#include <sgl/utilities/StringManager.h>

namespace
{
    constexpr int marginL = 20;
    constexpr int marginT = 15;

    constexpr int textSize = 18;
}

namespace game
{

PanelPlanetActionConquer::PanelPlanetActionConquer()
    : sgl::sgui::Widget(nullptr)
{
    using namespace sgl;

    auto tm = graphic::TextureManager::Instance();
    auto fm = graphic::FontManager::Instance();
    auto sm = utilities::StringManager::Instance();

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFilePlanetMap2, IND_PM_PANEL_ACTIONS);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    SetSize(tex->GetWidth(), tex->GetHeight());

    // TITLE
    graphic::Font * fnt = fm->GetFont(WidgetsConstants::FontFilePanelTitle,
                                      WidgetsConstants::FontSizePlanetMapTitle, graphic::Font::NORMAL);
    mTitle = new sgui::Label(sm->GetCString("CONQUER"), fnt, this);
    mTitle->SetColor(WidgetsConstants::colorPanelTitle);
    mTitle->SetPosition(marginL, marginT);

    // CONTENT
    CreateContentStart();

    // BUTTONS
    mButtonOk = new ButtonPlanetMap(this);
    mButtonOk->SetLabel(sm->GetCString("PROCEED"));

    mButtonCancel = new SecondaryButtonPlanetMap(this);
    mButtonCancel->SetLabel(sm->GetCString("CANCEL"));

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

    auto sm = utilities::StringManager::Instance();

    mContentStart = new sgui::Widget(this);

    auto fm = graphic::FontManager::Instance();

    const int w = GetWidth();

    // DESCRIPTION
    graphic::Font * fnt = fm->GetFont(WidgetsConstants::FontFileText, textSize, graphic::Font::NORMAL);

    const int marginL = 20;
    const int marginR = 20;
    const int contW = w - marginL - marginR;
    const int contH = 100;
    const char * txt = sm->GetCString("DESC_ACT_CONQUER");
    mDesc = new sgui::TextArea(contW, contH, txt, fnt, false, mContentStart);
    mDesc->SetColor(WidgetsConstants::colorPanelText);
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

    const int marginTextT = 20;

    int x;
    int y;

    // BACKGROUND
    mBg->SetPosition(x0, y0);

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

void PanelPlanetActionConquer::OnStringsChanged()
{
    auto sm = sgl::utilities::StringManager::Instance();

    mTitle->SetText(sm->GetCString("CONQUER"));

    mButtonOk->SetLabel(sm->GetCString("PROCEED"));
    mButtonCancel->SetLabel(sm->GetCString("CANCEL"));

    mDesc->SetText(sm->GetCString("DESC_ACT_CONQUER"));
}

} // namespace game
