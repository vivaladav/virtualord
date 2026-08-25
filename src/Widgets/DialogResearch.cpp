#include "Widgets/DialogResearch.h"

#include "Game.h"
#include "Player.h"
#include "GameObjects/ResearchCenter.h"
#include "Widgets/ButtonDialogClose.h"
#include "Widgets/GameSliderH.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/GraphicConstants.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

const int marginSide = 40;
const int marginButtonsB = 20;

DialogResearch::DialogResearch(Player * player, ResearchCenter * rc)
    : mPlayer(player)
    , mResCenter(rc)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    auto sm = utilities::StringManager::Instance();

    const int headerFontSize = 22;
    const unsigned int colorHeader = 0x9dcbe2ff;
    const unsigned int colorLabel = 0x70a7c2ff;

    // -- BACKGROUND --
    const int w = 850;
    graphic::Texture * tex;

    tex = tm->GetSprite(SpriteFileDialogResearch, ID_DLG_RES_BG_L);
    mBgL = new graphic::Image(tex);
    RegisterRenderable(mBgL);

    const int wL = mBgL->GetWidth();
    const int h = mBgL->GetHeight();

    tex = tm->GetSprite(SpriteFileDialogResearch, ID_DLG_RES_BG_R);
    mBgR = new graphic::Image(tex);
    RegisterRenderable(mBgR);

    const int wR = mBgR->GetWidth();

    tex = tm->GetTexture(SpriteFileDialogResearchExp);
    tex->SetScaleMode(graphic::TSCALE_NEAREST);
    mBgC = new graphic::Image(tex);
    RegisterRenderable(mBgC);

    const int wC = w - wL - wR;
    mBgC->SetWidth(wC);

    SetSize(w, h);

    // -- BUTTON CLOSE --
    mBtnClose = new ButtonDialogClose(this);

    const int buttonX = w - mBtnClose->GetWidth();
    mBtnClose->SetX(buttonX);

    // -- CONTENT --
    const int marginContL2 = 420;
    const int marginContR = 40;
    const int marginHeaderB = 25;
    const int marginIconR = 25;
    const int marginIconB = 60;
    const int marginSliderR = 20;

    const int minRes = 0;
    const int topRes = 100;
    const int topRes2 = 10;
    const int stepRes = 10;
    const int stepSingle = 1;

    auto fontHeader = fm->GetFont(WidgetsConstants::FontFileHeader, 20, graphic::Font::NORMAL);
    auto fontText = fm->GetFont(WidgetsConstants::FontFileText, 18, graphic::Font::NORMAL);

    auto texSliderBg = tm->GetSprite(SpriteFileUIShared, ID_SLIDER_DLG_BG);
    auto texSliderBar = tm->GetSprite(SpriteFileUIShared, ID_SLIDER_DLG_BAR);
    auto texSliderBtn = tm->GetSprite(SpriteFileUIShared, ID_SLIDER_DLG_BTN);

    int valRes = 0;

    // TITLE
    auto fontTitle = fm->GetFont(WidgetsConstants::FontFileDialogTitle,
                                 WidgetsConstants::FontSizeDialogTitle, graphic::Font::NORMAL);

    sgui::Label * title = new sgui::Label(sm->GetCString("RESEARCH"), fontTitle, this);

    const int titleX = WidgetsConstants::MarginDialogTitleL;
    const int titleY = (WidgetsConstants::DialogTitleBarH - title->GetHeight()) / 2;
    title->SetPosition(titleX, titleY);
    title->SetColor(WidgetsConstants::colorDialogTitle);

    int x = WidgetsConstants::MarginDialogContentL;
    int y = WidgetsConstants::DialogTitleBarH + WidgetsConstants::MarginDialogContentT;

    // HEADER INVEST
    auto header = new sgui::Label(sm->GetCString("INVEST_RES"), fontHeader, this);
    header->SetColor(WidgetsConstants::colorDialogHeader1);
    header->SetPosition(x, y);

    y += header->GetHeight() + marginHeaderB;

    // MONEY
    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_MONEY_24);
    auto icon = new sgui::Image(tex, this);
    icon->SetPosition(x, y);

    x += icon->GetWidth() + marginIconR;

    valRes = mResCenter->GetWantedResourceUsage(ER_MONEY);

    if(valRes > topRes)
        valRes = topRes;

    mSliderMoney = new GameSliderH(texSliderBg, texSliderBar, texSliderBtn, this);

    y += (icon->GetHeight() - mSliderMoney->GetHeight()) / 2;

    mSliderMoney->SetMinMax(minRes, topRes);
    mSliderMoney->SetValue(valRes);
    mSliderMoney->SetPosition(x, y);

    mSliderMoney->SetStep(stepRes);

    x += mSliderMoney->GetWidth() + marginSliderR;

    auto label = new sgui::Label(std::to_string(valRes).c_str(), fontText, this);

    y = icon->GetY() + (icon->GetHeight() - label->GetHeight()) / 2;

    label->SetColor(WidgetsConstants::colorDialogText);
    label->SetPosition(x, y);

    mSliderMoney->AddOnValueChanged([this, label](int val)
    {
        label->SetText(std::to_string(val).c_str());

        mResCenter->SetWantedResourceUsage(ER_MONEY, val);
        UpdateOutput();
    });

    // ENERGY
    x = WidgetsConstants::MarginDialogContentL;
    y = icon->GetY() + icon->GetHeight() + marginIconB;

    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_ENERGY_24);
    icon = new sgui::Image(tex, this);
    icon->SetPosition(x, y);

    x += icon->GetWidth() + marginIconR;

    valRes = mResCenter->GetWantedResourceUsage(ER_ENERGY);

    if(valRes > topRes)
        valRes = topRes;

    mSliderEnergy = new GameSliderH(texSliderBg, texSliderBar, texSliderBtn, this);

    y += (icon->GetHeight() - mSliderEnergy->GetHeight()) / 2;

    mSliderEnergy->SetMinMax(minRes, topRes);
    mSliderEnergy->SetValue(valRes);
    mSliderEnergy->SetPosition(x, y);

    mSliderEnergy->SetStep(stepRes);

    x += mSliderEnergy->GetWidth() + marginSliderR;

    label = new sgui::Label(std::to_string(valRes).c_str(), fontText, this);

    y = icon->GetY() + (icon->GetHeight() - label->GetHeight()) / 2;

    label->SetColor(WidgetsConstants::colorDialogText);
    label->SetPosition(x, y);

    mSliderEnergy->AddOnValueChanged([this, label](int val)
    {
        label->SetText(std::to_string(val).c_str());

        mResCenter->SetWantedResourceUsage(ER_ENERGY, val);
        UpdateOutput();
    });

    // MATERIAL
    x = WidgetsConstants::MarginDialogContentL;
    y = icon->GetY() + icon->GetHeight() + marginIconB;

    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_MATERIAL_24);
    icon = new sgui::Image(tex, this);
    icon->SetPosition(x, y);

    x += icon->GetWidth() + marginIconR;

    valRes = mResCenter->GetWantedResourceUsage(ER_MATERIAL);

    if(valRes > topRes)
        valRes = topRes;

    mSliderMaterial = new GameSliderH(texSliderBg, texSliderBar, texSliderBtn, this);

    y += (icon->GetHeight() - mSliderMaterial->GetHeight()) / 2;

    mSliderMaterial->SetMinMax(minRes, topRes);
    mSliderMaterial->SetValue(valRes);
    mSliderMaterial->SetPosition(x, y);

    mSliderMaterial->SetStep(stepRes);

    x += mSliderMaterial->GetWidth() + marginSliderR;

    label = new sgui::Label(std::to_string(valRes).c_str(), fontText, this);

    y = icon->GetY() + (icon->GetHeight() - label->GetHeight()) / 2;

    label->SetColor(WidgetsConstants::colorDialogText);
    label->SetPosition(x, y);

    mSliderMaterial->AddOnValueChanged([this, label](int val)
    {
        label->SetText(std::to_string(val).c_str());

        mResCenter->SetWantedResourceUsage(ER_MATERIAL, val);
        UpdateOutput();
    });

    // DIAMONDS
    x = marginContL2;
    y = header->GetY() + header->GetHeight() + marginHeaderB;

    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_DIAMONDS_24);
    icon = new sgui::Image(tex, this);
    icon->SetPosition(x, y);

    x += icon->GetWidth() + marginIconR;

    valRes = mResCenter->GetWantedResourceUsage(ER_DIAMONDS);

    if(valRes > topRes2)
        valRes = topRes2;

    mSliderDiamonds = new GameSliderH(texSliderBg, texSliderBar, texSliderBtn, this);

    y += (icon->GetHeight() - mSliderDiamonds->GetHeight()) / 2;

    mSliderDiamonds->SetMinMax(minRes, topRes2);
    mSliderDiamonds->SetValue(valRes);
    mSliderDiamonds->SetPosition(x, y);
    mSliderDiamonds->SetStep(stepSingle);

    x += mSliderDiamonds->GetWidth() + marginSliderR;

    label = new sgui::Label(std::to_string(valRes).c_str(), fontText, this);

    y = icon->GetY() + (icon->GetHeight() - label->GetHeight()) / 2;

    label->SetColor(WidgetsConstants::colorDialogText);
    label->SetPosition(x, y);

    mSliderDiamonds->AddOnValueChanged([this, label](int val)
    {
        label->SetText(std::to_string(val).c_str());

        mResCenter->SetWantedResourceUsage(ER_DIAMONDS, val);
        UpdateOutput();
    });

    // BLOBS
    x = marginContL2;
    y = icon->GetY() + icon->GetHeight() + marginIconB;

    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_BLOBS_24);
    icon = new sgui::Image(tex, this);
    icon->SetPosition(x, y);

    x += icon->GetWidth() + marginIconR;

    valRes = mResCenter->GetWantedResourceUsage(ER_BLOBS);

    if(valRes > topRes2)
        valRes = topRes2;

    mSliderBlobs = new GameSliderH(texSliderBg, texSliderBar, texSliderBtn, this);

    y += (icon->GetHeight() - mSliderBlobs->GetHeight()) / 2;

    mSliderBlobs->SetMinMax(minRes, topRes2);
    mSliderBlobs->SetValue(valRes);
    mSliderBlobs->SetPosition(x, y);
    mSliderBlobs->SetStep(stepSingle);

    x += mSliderBlobs->GetWidth() + marginSliderR;

    label = new sgui::Label(std::to_string(valRes).c_str(), fontText, this);

    y = icon->GetY() + (icon->GetHeight() - label->GetHeight()) / 2;

    label->SetColor(WidgetsConstants::colorDialogText);
    label->SetPosition(x, y);

    mSliderBlobs->AddOnValueChanged([this, label](int val)
    {
        label->SetText(std::to_string(val).c_str());

        mResCenter->SetWantedResourceUsage(ER_BLOBS, val);
        UpdateOutput();
    });

    // HEADER RESEARCH / TURN
    const int header2Y = 400;

    x = WidgetsConstants::MarginDialogContentL;
    y = header2Y;

    header = new sgui::Label(sm->GetCString("RESEARCH_PTS_TURN"), fontHeader, this);
    header->SetColor(WidgetsConstants::colorDialogHeader1);
    header->SetPosition(x, y);

    // RESEARCH POINTS
    const int researchPts = mResCenter->GetResourceProduction(ER_RESEARCH);
    mLabelProd = new sgui::Label(std::to_string(researchPts).c_str(), fontText, this);
    mLabelProd->SetColor(WidgetsConstants::colorDialogText);

    x = w - marginContR - mLabelProd->GetWidth();
    y += (header->GetHeight() - mLabelProd->GetHeight()) / 2;
    mLabelProd->SetPosition(x, y);

    // ICON RESEARCH
    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_RESEARCH_24);
    icon = new sgui::Image(tex, this);

    x -= marginIconR + icon->GetWidth();
    y = mLabelProd->GetY() + (mLabelProd->GetHeight() - icon->GetHeight()) / 2;

    icon->SetPosition(x, y);
}

void DialogResearch::SetFunctionOnClose(const std::function<void()> & f)
{
    mBtnClose->AddOnClickFunction(f);
}

void DialogResearch::HandlePositionChanged()
{
    SetPositions();
}

void DialogResearch::SetPositions()
{
    const int y = GetScreenY();
    int x = GetScreenX();

    // BACKGROUND
    mBgL->SetPosition(x, y);
    x += mBgL->GetWidth();

    mBgC->SetPosition(x, y);
    x += mBgC->GetWidth();

    mBgR->SetPosition(x, y);
}

void DialogResearch::UpdateOutput()
{
    const int researchPts = mResCenter->GetResourceProduction(ER_RESEARCH);
    mLabelProd->SetText(std::to_string(researchPts).c_str());
}

} // namespace game
