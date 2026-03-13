#include "ResourceTooltip.h"

#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/Texture.h>
#include <sgl/sgui/Label.h>

namespace game
{

constexpr unsigned int colorLabels = 0x84b7e1ff;

ResourceTooltip::ResourceTooltip(const char * title)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFileTooltips, ID_TOOLTIP_RES_BAR_BG);

    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    SetSize(mBg->GetWidth(), mBg->GetHeight());

    // TITLE
    SetTitle(title);

    // LABELS
    auto fontLabel = fm->GetFont(WidgetsConstants::FontFileText, 18, graphic::Font::NORMAL);

    mLabelIn = new sgui::Label(fontLabel, this);
    mLabelIn->SetColor(colorLabels);

    mLabelOut = new sgui::Label(fontLabel, this);
    mLabelOut->SetColor(colorLabels);

    mLabelTot = new sgui::Label(fontLabel, this);
    mLabelTot->SetColor(colorLabels);

    // init labels
    SetValues(0, 0);
}

void ResourceTooltip::SetTitle(const char * text)
{
    using namespace sgl;

    const bool init = mTitle == nullptr;

    if(!init)
    {
        UnregisterRenderable(mTitle);
        delete mTitle;
    }

    auto fm = graphic::FontManager::Instance();
    auto fontTitle = fm->GetFont(WidgetsConstants::FontFilePanelTitle, 18,
                                 graphic::Font::NORMAL);

    mTitle = new graphic::Text(text, fontTitle);
    mTitle->SetColor(WidgetsConstants::colorTooltipTitle);
    RegisterRenderable(mTitle);

    if(!init)
        SetPositions();
}

void ResourceTooltip::SetValues(int resIn, int resOut)
{
    if(resIn == mIn && resOut == mOut)
        return ;

    const int w = GetWidth();
    const int marginTop1 = 43;
    const int marginTop2 = marginTop1 + 40;
    const int marginTop3 = marginTop2 + 40;
    const int marginRight = 25;

    const unsigned int colorPos = 0x6ef785ff;
    const unsigned int colorNeg = 0xf2948cff;

    const int diff = resIn - resOut;

    int posX;

    // IN
    const std::string strIn = GetSignedNumber(resIn);
    mLabelIn->SetText(strIn.c_str());

    posX = w - marginRight - mLabelIn->GetWidth();
    mLabelIn->SetPosition(posX, marginTop1);

    // OUT
    const std::string strOut = GetSignedNumber(-resOut);
    mLabelOut->SetText(strOut.c_str());

    posX = w - marginRight - mLabelOut->GetWidth();
    mLabelOut->SetPosition(posX, marginTop2);

    // TOT
    const std::string strDiff = GetSignedNumber(diff);
    mLabelTot->SetText(strDiff.c_str());

    posX = w - marginRight - mLabelTot->GetWidth();
    mLabelTot->SetPosition(posX, marginTop3);

    if(diff > 0)
        mLabelTot->SetColor(colorPos);
    else if(diff < 0)
        mLabelTot->SetColor(colorNeg);
    else
        mLabelTot->SetColor(colorLabels);

    // UPDATE VALUES
    mIn = resIn;
    mOut = resOut;
}

void ResourceTooltip::HandlePositionChanged()
{
    SetPositions();
}

void ResourceTooltip::SetPositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();
    const int w = GetWidth();

    // BACKGROUND
    mBg->SetPosition(x0, y0);

    // LABEL
    const int marginTop = 10;
    const int labelX = x0 + (w - mTitle->GetWidth()) / 2;
    const int labelY = y0 + marginTop;

    mTitle->SetPosition(labelX, labelY);
}

std::string ResourceTooltip::GetSignedNumber(int val)
{
    const std::string plus("+");
    const std::string zero("0");

    // IN
    std::string str;

    if(val > 0)
        str = plus + std::to_string(val);
    else if(val < 0)
        str = std::to_string(val);
    else
        str = zero;

    return str;
}

} // namespace game
