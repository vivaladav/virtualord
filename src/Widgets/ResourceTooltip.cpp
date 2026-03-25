#include "ResourceTooltip.h"

#include "GameConstants.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/Texture.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringManager.h>

#include <sstream>

namespace
{
    const char * textHeader1[] =
    {
        "ENERGY",
        "MATERIAL",
        "DIAMONDS",
        "BLOBS",
        "MONEY",
        "RESEARCH_PTS",
    };

    const char * textHeader2[] =
    {
        "ENERGY_TURN",
        "MATERIAL_TURN",
        "DIAMONDS_TURN",
        "BLOBS_TURN",
        "MONEY_TURN",
        "RESEARCH_TURN",
    };

constexpr int marginContH = 42;
}

namespace game
{

ResourceTooltip::ResourceTooltip(ExtendedResource res)
    : mRes(res)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    auto sm = utilities::StringManager::Instance();

    const int sizeHeader = 18;
    auto fontHeader = fm->GetFont(WidgetsConstants::FontFilePanelHeader, sizeHeader,
                                  graphic::Font::NORMAL);

    const int marginT = 15;
    const int marginHeaderB = 5;
    const int marginValB = 5;
    const int marginValB2 = 15;
    const int marginBlock1B = 25;

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFileTooltips, ID_TOOLTIP_RES_BAR_BG);

    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    const int w = mBg->GetWidth();
    const int contW = w - (2 * marginContH);
    const int h = mBg->GetHeight();
    SetSize(w, h);

    // HEADER 1
    mHeader1 = new sgui::Label(sm->GetCString(textHeader1[res]), fontHeader, this);

    int x = (w - mHeader1->GetWidth()) / 2;
    int y = marginT;

    mHeader1->SetPosition(x, y);

    // LABEL VAL
    const int sizeLabel = 18;
    const char * defText = "0";
    auto fontLabel = fm->GetFont(WidgetsConstants::FontFileText, sizeLabel, graphic::Font::NORMAL);

    mLabelVal = new sgui::Label(defText, fontLabel, this);
    mLabelVal->SetColor(WidgetsConstants::colorPanelText);

    x = (w - mLabelVal->GetWidth()) / 2;
    y += mHeader1->GetHeight() + marginHeaderB;
    mLabelVal->SetPosition(x, y);

    // HEADER 2
    mHeader2 = new sgui::Label(sm->GetCString(textHeader2[res]), fontHeader, this);

    x = (w - mHeader2->GetWidth()) / 2;
    y += mLabelVal->GetHeight() + marginBlock1B;
    mHeader2->SetPosition(x, y);

    // LABEL IN
    mLabelIn = new sgui::Label(defText, fontLabel, this);
    mLabelIn->SetColor(WidgetsConstants::colorPanelText);

    x = marginContH + contW - mLabelIn->GetWidth();
    y += mHeader2->GetHeight() + marginHeaderB;
    mLabelIn->SetPosition(x, y);

    // LABEL OUT
    mLabelOut = new sgui::Label(defText, fontLabel, this);
    mLabelOut->SetColor(WidgetsConstants::colorPanelText);

    x = marginContH + contW - mLabelOut->GetWidth();
    y += mLabelIn->GetHeight() + marginValB;
    mLabelOut->SetPosition(x, y);

    // LABEL TOT
    mLabelTot = new sgui::Label(defText, fontLabel, this);

    x = marginContH + contW - mLabelTot->GetWidth();
    y += mLabelOut->GetHeight() + marginValB2;

    mLabelTot->SetPosition(x, y);
}

void ResourceTooltip::UpdateStrings()
{
    auto sm = sgl::utilities::StringManager::Instance();

    mHeader1->SetText(sm->GetCString(textHeader1[mRes]));
    mHeader2->SetText(sm->GetCString(textHeader2[mRes]));
}

void ResourceTooltip::SetValues(int val, int maxVal, int resIn, int resOut)
{
    if(resIn == mIn && resOut == mOut)
        return ;

    const int w = GetWidth();
    const int contW = w - (2 * marginContH);

    const int diff = resIn - resOut;

    int x;

    // VALUE
    if(mRes == ER_MONEY || mRes == ER_RESEARCH)
        mLabelVal->SetText(std::to_string(val).c_str());
    else
    {
        const std::string sep("  / ");
        std::ostringstream ss;
        ss << val << sep << maxVal;

        mLabelVal->SetText(ss.str().c_str());
    }

    x = (w - mLabelVal->GetWidth()) / 2;
    mLabelVal->SetX(x);

    // IN
    const std::string strIn = GetSignedNumber(resIn);
    mLabelIn->SetText(strIn.c_str());

    x = marginContH + contW - mLabelIn->GetWidth();
    mLabelIn->SetX(x);

    // OUT
    const std::string strOut = GetSignedNumber(-resOut);
    mLabelOut->SetText(strOut.c_str());

    x = marginContH + contW - mLabelOut->GetWidth();
    mLabelOut->SetX(x);

    // TOT
    const std::string strDiff = GetSignedNumber(diff);
    mLabelTot->SetText(strDiff.c_str());

    x = marginContH + contW - mLabelTot->GetWidth();
    mLabelTot->SetX(x);

    if(diff > 0)
        mLabelTot->SetColor(WidgetsConstants::colorPanelGood);
    else if(diff < 0)
        mLabelTot->SetColor(WidgetsConstants::colorPanelBad);
    else
        mLabelTot->SetColor(WidgetsConstants::colorPanelText);

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
    // BACKGROUND
    mBg->SetPosition(GetScreenX(), GetScreenY());
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
