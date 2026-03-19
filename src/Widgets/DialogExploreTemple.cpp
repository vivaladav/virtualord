#include "Widgets/DialogExploreTemple.h"

#include "Game.h"
#include "Player.h"
#include "GameObjects/Temple.h"
#include "Widgets/ButtonDialogClose.h"
#include "Widgets/GameButton.h"
#include "Widgets/GameSliderH.h"
#include "Widgets/GameUIData.h"
#include "Widgets/WidgetsConstants.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/GraphicConstants.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/Texture.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>
#include <sgl/sgui/TextArea.h>
#include <sgl/utilities/StringManager.h>

#include <sstream>

// anonymous namespace for local "private" classes
namespace
{

using namespace game;

// ===== BUTTON =====
class ButtonExploreTemple : public GameButton
{
public:
    ButtonExploreTemple(sgl::sgui::Widget * parent)
        : GameButton(SpriteFileDialogExploreTemple,
                     { ID_DLG_EXTM_BTN_NORMAL, ID_DLG_EXTM_BTN_DISABLED,
                       ID_DLG_EXTM_BTN_OVER, ID_DLG_EXTM_BTN_PUSHED, ID_DLG_EXTM_BTN_PUSHED },
                     { WidgetsConstants::colorDialogButtonOkNormal,
                       WidgetsConstants::colorDialogButtonOkDisabled,
                       WidgetsConstants::colorDialogButtonOkOver,
                       WidgetsConstants::colorDialogButtonOkPushed,
                       WidgetsConstants::colorDialogButtonOkChecked },
                     parent)
    {
        using namespace sgl;

        const int size = 22;

        auto fm = graphic::FontManager::Instance();
        auto fnt = fm->GetFont(WidgetsConstants::FontFileButton, size, graphic::Font::NORMAL);
        SetLabelFont(fnt);
    }

    void HandleMouseOver() override
    {
        sgl::sgui::AbstractButton::HandleMouseOver();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_over-01.ogg");
    }

    void HandleButtonDown() override
    {
        sgl::sgui::AbstractButton::HandleButtonDown();

        auto player = sgl::media::AudioManager::Instance()->GetPlayer();
        player->PlaySound("UI/button_click-01.ogg");
    }
};

} // namespace

namespace game
{

// ===== DIALOG EXPLORE TEMPLE =====
DialogExploreTemple::DialogExploreTemple(Player * player, Temple * temple)
    : mPlayer(player)
    , mTemple(temple)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    auto sm = utilities::StringManager::Instance();

    // -- BACKGROUND --
    const int w = 904;
    graphic::Texture * tex;

    tex = tm->GetSprite(SpriteFileDialogExploreTemple, ID_DLG_EXTM_BG_L);
    mBgL = new graphic::Image(tex);
    RegisterRenderable(mBgL);

    const int wL = mBgL->GetWidth();
    const int h = mBgL->GetHeight();

    tex = tm->GetSprite(SpriteFileDialogExploreTemple, ID_DLG_EXTM_BG_R);
    mBgR = new graphic::Image(tex);
    RegisterRenderable(mBgR);

    const int wR = mBgR->GetWidth();

    tex = tm->GetSprite(SpriteFileDialogExploreTempleExp, ID_DLG_EXTM_BG_C);
    tex->SetScaleMode(graphic::TSCALE_NEAREST);
    mBgC = new graphic::Image(tex);
    RegisterRenderable(mBgC);

    const int wC = w - wL - wR;
    mBgC->SetWidth(wC);

    SetSize(w, h);

    // BUTTON CLOSE
    mBtnClose = new ButtonDialogClose(this);
    mBtnClose->SetX(w - mBtnClose->GetWidth());

    // TITLE
    auto fontTitle = fm->GetFont(WidgetsConstants::FontFileDialogTitle,
                                 WidgetsConstants::FontSizeDialogTitle, graphic::Font::NORMAL);

    auto title = new sgui::Label(sm->GetCString("EXPL_TEMPLE"), fontTitle, this);

    const int titleX = WidgetsConstants::MarginDialogTitleL;
    const int titleY = (WidgetsConstants::DialogTitleBarH - title->GetHeight()) / 2;
    title->SetPosition(titleX, titleY);
    title->SetColor(WidgetsConstants::colorDialogTitle);

    // -- CONTENT --
    const int headerSize = 22;
    const int textSize = 18;

    auto fontHeader = fm->GetFont(WidgetsConstants::FontFileHeader, headerSize, graphic::Font::NORMAL);
    auto fontLabel = fm->GetFont(WidgetsConstants::FontFileText, textSize, graphic::Font::NORMAL);

    const int marginHeaderB = 20;
    const int marginIconR = 20;
    const int marginSliderR = 20;
    const int marginResourceB = 55;
    const int marginPanelV = 40;
    const int barH = 3;
    const int blockResW = 510;
    const int minInvest = 0;

    auto texSliderBg = tm->GetSprite(SpriteFileUIShared, ID_SLIDER_DLG_BG);
    auto texSliderBar = tm->GetSprite(SpriteFileUIShared, ID_SLIDER_DLG_BAR);
    auto texSliderBtn = tm->GetSprite(SpriteFileUIShared, ID_SLIDER_DLG_BTN);

    int x = WidgetsConstants::MarginDialogContentL;
    int y = WidgetsConstants::DialogTitleBarH + WidgetsConstants::MarginDialogContentT;

    // HEADER INVEST
    auto header = new sgui::Label(sm->GetCString("INVEST_RES"), fontHeader, this);
    header->SetColor(WidgetsConstants::colorDialogHeader2);
    header->SetPosition(x, y);

    y += header->GetHeight() + marginHeaderB;

    // MONEY - ICON
    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_MONEY_24);
    auto icon = new sgui::Image(tex, this);
    icon->SetPosition(x, y);

    // MONEY - SLIDER
    const int playerMoney = player->GetStat(Player::Stat::MONEY).GetValue();
    const int maxTempleMoney = temple->GetMaxInvestableMoney();
    const int maxMoney =  playerMoney < maxTempleMoney ? playerMoney : maxTempleMoney;

    mSliderMoney = new GameSliderH(texSliderBg, texSliderBar, texSliderBtn, this);
    mSliderMoney->SetMinMax(minInvest, maxMoney);
    mSliderMoney->SetValue(minInvest + (maxMoney - minInvest) / 2);
    mSliderMoney->SetPosition(icon->GetX() + icon->GetWidth() + marginIconR,
                              icon->GetY() + (icon->GetHeight() - mSliderMoney->GetHeight()) / 2);

    auto label = new sgui::Label(std::to_string(mSliderMoney->GetValue()).c_str(), fontLabel, this);
    label->SetColor(WidgetsConstants::colorDialogText);
    label->SetPosition(mSliderMoney->GetX() + mSliderMoney->GetWidth() + marginSliderR,
                       mSliderMoney->GetY());

    mSliderMoney->SetOnValueChanged([this, label](int val)
    {
        label->SetText(std::to_string(val).c_str());

        OnInvestmentChanged();
    });

    y += icon->GetHeight() + marginResourceB;

    // MATERIAL - ICON
    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_MATERIAL_24);
    icon = new sgui::Image(tex, this);
    icon->SetPosition(x, y);

    // MATERIAL - SLIDER
    const int playerMaterial = player->GetStat(Player::Stat::MATERIAL).GetValue();
    const int maxTempleMaterial = temple->GetMaxInvestableMaterial();
    const int maxMaterial =  playerMaterial < maxTempleMaterial ? playerMaterial : maxTempleMaterial;

    mSliderMaterial = new GameSliderH(texSliderBg, texSliderBar, texSliderBtn, this);
    mSliderMaterial->SetMinMax(minInvest, maxMaterial);
    mSliderMaterial->SetValue(minInvest + (maxMaterial - minInvest) / 2);
    mSliderMaterial->SetPosition(icon->GetX() + icon->GetWidth() + marginIconR,
                                 icon->GetY() + (icon->GetHeight() - mSliderMaterial->GetHeight()) / 2);

    label = new sgui::Label(std::to_string(mSliderMaterial->GetValue()).c_str(), fontLabel, this);
    label->SetColor(WidgetsConstants::colorDialogText);
    label->SetPosition(mSliderMaterial->GetX() + mSliderMaterial->GetWidth() + marginSliderR,
                       mSliderMaterial->GetY());

    mSliderMaterial->SetOnValueChanged([this, label](int val)
    {
        label->SetText(std::to_string(val).c_str());

        OnInvestmentChanged();
    });

    x += blockResW;
    y = header->GetY() + header->GetHeight() + marginHeaderB;

    // DIAMONDS - ICON
    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_DIAMONDS_24);
    icon = new sgui::Image(tex, this);
    icon->SetPosition(x, y);

    // DIAMONDS - SLIDER
    const int playerDiamonds = player->GetStat(Player::Stat::DIAMONDS).GetValue();
    const int maxTempleDiamonds = temple->GetMaxInvestableBlobs();
    const int maxDiamonds =  playerDiamonds < maxTempleDiamonds ? playerDiamonds : maxTempleDiamonds;

    mSliderDiamonds = new GameSliderH(texSliderBg, texSliderBar, texSliderBtn, this);
    mSliderDiamonds->SetMinMax(minInvest, maxDiamonds);
    mSliderDiamonds->SetValue(minInvest + (maxDiamonds - minInvest) / 2);
    mSliderDiamonds->SetPosition(icon->GetX() + icon->GetWidth() + marginIconR,
                                 icon->GetY() + (icon->GetHeight() - mSliderDiamonds->GetHeight()) / 2);

    label = new sgui::Label(std::to_string(mSliderDiamonds->GetValue()).c_str(), fontLabel, this);
    label->SetColor(WidgetsConstants::colorDialogText);
    label->SetPosition(mSliderDiamonds->GetX() + mSliderDiamonds->GetWidth() + marginSliderR,
                       mSliderDiamonds->GetY());

    mSliderDiamonds->SetOnValueChanged([this, label](int val)
    {
        label->SetText(std::to_string(val).c_str());

        OnInvestmentChanged();
    });

    y += icon->GetHeight() + marginResourceB;

    // BLOBS - ICON
    tex = tm->GetSprite(SpriteFileUIShared, ID_UIS_ICON_C_RES_BLOBS_24);
    icon = new sgui::Image(tex, this);
    icon->SetPosition(x, y);


    // BLOBS - SLIDER
    const int playerBlobs = player->GetStat(Player::Stat::BLOBS).GetValue();
    const int maxTempleBlobs = temple->GetMaxInvestableBlobs();
    const int maxBlobs =  playerBlobs < maxTempleBlobs ? playerBlobs : maxTempleBlobs;

    mSliderBlobs = new GameSliderH(texSliderBg, texSliderBar, texSliderBtn, this);
    mSliderBlobs->SetMinMax(minInvest, maxBlobs);
    mSliderBlobs->SetValue(minInvest + (maxBlobs - minInvest) / 2);
    mSliderBlobs->SetPosition(icon->GetX() + icon->GetWidth() + marginIconR,
                              icon->GetY() + (icon->GetHeight() - mSliderBlobs->GetHeight()) / 2);

    label = new sgui::Label(std::to_string(mSliderBlobs->GetValue()).c_str(), fontLabel, this);
    label->SetColor(WidgetsConstants::colorDialogText);
    label->SetPosition(mSliderBlobs->GetX() + mSliderBlobs->GetWidth() + marginSliderR,
                       mSliderBlobs->GetY());

    mSliderBlobs->SetOnValueChanged([this, label](int val)
    {
        label->SetText(std::to_string(val).c_str());

        OnInvestmentChanged();
    });

    // -- OUTCOME PANEL --
    const int headerSize2 = 24;
    const int textSize2 = 20;

    fontHeader = fm->GetFont(WidgetsConstants::FontFileHeader, headerSize2, graphic::Font::NORMAL);
    fontLabel = fm->GetFont(WidgetsConstants::FontFileText, textSize2, graphic::Font::NORMAL);

    const int marginHeaderOutcomeB = 10;
    y += (2 * marginPanelV) + barH;

    header = new sgui::Label(sm->GetCString("TURNS_REQUIRED"), fontHeader, this);
    header->SetColor(WidgetsConstants::colorDialogHeader2);
    x = (w / 4) - (header->GetWidth() / 2);
    header->SetPosition(x, y);

    y += header->GetHeight() + marginHeaderOutcomeB;

    mLabelTurns = new sgui::Label(fontLabel, this);
    mLabelTurns->SetColor(WidgetsConstants::colorDialogText);
    mLabelTurns->SetY(y);

    y = header->GetY();

    header = new sgui::Label(sm->GetCString("SUCCESS_PROB"), fontHeader, this);
    header->SetColor(WidgetsConstants::colorDialogHeader2);
    x = w - (w / 4) - (header->GetWidth() / 2);
    header->SetPosition(x, y);

    y += header->GetHeight() + marginHeaderOutcomeB;

    mLabelSuccess = new sgui::Label(fontLabel, this);
    mLabelSuccess->SetColor(WidgetsConstants::colorDialogText);
    mLabelSuccess->SetY(y);

    // BUTTON EXPLORE
    mBtnExplore = new ButtonExploreTemple(this);
    mBtnExplore->SetLabel(sm->GetCString("EXPLORE"));
    mBtnExplore->SetPosition((w - mBtnExplore->GetWidth()) / 2,
                             h - mBtnExplore->GetHeight() - WidgetsConstants::MarginDialogContentB);

    // INIT VALUES
    OnInvestmentChanged();
}

void DialogExploreTemple::SetFunctionOnCancel(const std::function<void()> & f)
{
    mBtnClose->AddOnClickFunction(f);
}

void DialogExploreTemple::SetFunctionOnExplore(const std::function<void()> & f)
{
    mBtnExplore->AddOnClickFunction(f);
}

void DialogExploreTemple::HandlePositionChanged()
{
    SetPositions();
}

void DialogExploreTemple::OnInvestmentChanged()
{
    const int w = GetWidth();

    // update invested resources
    mTemple->SetInvestedResources(mSliderMoney->GetValue(), mSliderMaterial->GetValue(),
                                  mSliderBlobs->GetValue(), mSliderDiamonds->GetValue());

    // update turns
    const int turns = mTemple->GetExplorationTurns();

    mLabelTurns->SetText(std::to_string(turns).c_str());
    mLabelTurns->SetX((w / 4) - (mLabelTurns->GetWidth() / 2));

    // update success rate
    const int success = mTemple->GetExplorationSuccessRate();

    std::ostringstream ss;
    ss << success << "%";

    mLabelSuccess->SetText(ss.str().c_str());
    mLabelSuccess->SetX(w - (w / 4) - (mLabelSuccess->GetWidth() / 2));
}

void DialogExploreTemple::SetPositions()
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

// ===== DIALOG EXPLORE TEMPLE OUTCOME =====
DialogExploreTempleOutcome::DialogExploreTempleOutcome(Player * player, Temple * temple)
    : mPlayer(player)
    , mTemple(temple)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    auto sm = utilities::StringManager::Instance();

    const int headerFontSize = 22;
    const int labelFontSize = 22;
    const unsigned int colorHeader = 0x9dcbe2ff;
    const unsigned int colorLabel = 0x70a7c2ff;

    const int marginPanel1V = 85;

    auto fontLabel = fm->GetFont(WidgetsConstants::FontFileText, labelFontSize, graphic::Font::NORMAL);

    const Temple::ExplorationOutcomeCategory oc = temple->GetExplorationOutcomeCategory();

    // -- BACKGROUND --
    const int w = 904;
    graphic::Texture * tex;

    tex = tm->GetSprite(SpriteFileDialogExploreTemple, ID_DLG_EXTM_BG_L);
    mBgL = new graphic::Image(tex);
    RegisterRenderable(mBgL);

    const int wL = mBgL->GetWidth();
    const int h = mBgL->GetHeight();

    tex = tm->GetSprite(SpriteFileDialogExploreTemple, ID_DLG_EXTM_BG_R);
    mBgR = new graphic::Image(tex);
    RegisterRenderable(mBgR);

    const int wR = mBgR->GetWidth();

    tex = tm->GetSprite(SpriteFileDialogExploreTempleExp, ID_DLG_EXTM_BG_C);
    tex->SetScaleMode(graphic::TSCALE_NEAREST);
    mBgC = new graphic::Image(tex);
    RegisterRenderable(mBgC);

    const int wC = w - wL - wR;
    mBgC->SetWidth(wC);

    SetSize(w, h);

    // TITLE
    auto fontTitle = fm->GetFont(WidgetsConstants::FontFileDialogTitle, 32,
                                 graphic::Font::NORMAL);

    sgui::Label * title = new sgui::Label(sm->GetCString("TEMPLE_OUTCOME"),
                                          fontTitle, this);

    const int titleX = (w - title->GetWidth()) / 2;
    const int titleY = 10;
    title->SetPosition(titleX, titleY);
    title->SetColor(WidgetsConstants::colorDialogTitle);

    /*
    // EXPLANATION
    const int descAreaW = w - (marginSide * 2);
    const int descAreaH = 115;

    auto textDesc = new sgui::TextArea(descAreaW, descAreaH, fontLabel, false, this);
    textDesc->SetColor(colorLabel);

    const int textX = marginSide;
    const int textY = marginPanel1V;
    textDesc->SetPosition(textX, textY);

    // EXPLORATION GAVE NOTHING
    if(oc == Temple::EXP_OUTC_NOTHING)
    {
        textDesc->SetText(sm->GetCString("TO_DESC_NOTHING"));

        mBtnClose = new ButtonExploreTemple(this);
        mBtnClose->SetLabel(sm->GetCString("LEAVE"));

        const int buttonX = (w - mBtnClose->GetWidth()) / 2;
        const int buttonY = h - marginButtonsB - mBtnClose->GetHeight();
        mBtnClose->SetPosition(buttonX, buttonY);
    }
    // EXPLORATION GAVE REWARD OR PUNISHMENT
    else
    {
        const Temple::ExplorationOutcome o1 = temple->GetExplorationOutcome1();
        const Temple::ExplorationOutcome o2 = temple->GetExplorationOutcome2();

        // VERTICAL BAR
        tex = tm->GetSprite(SpriteFileDialogExploreTemple, ID_DLG_EXTM_LINE_V);
        mLine = new graphic::Image(tex);
        RegisterRenderable(mLine);

        // -- BUTTONS --
        mBtnOutcome1 = new ButtonExploreTemple(this);
        mBtnOutcome2 = new ButtonExploreTemple(this);

        // -- HEADER --
        auto fontHeader = fm->GetFont(WidgetsConstants::FontFileHeader, headerFontSize,
                                      graphic::Font::NORMAL);

        // -- OUTCOME TEXT --
        const int outcAreaW = w / 2 - (marginSide * 2);
        const int outcAreaH = 150;

        const char * desc1 = temple->GetExplorationOutcomeString(o1);
        mDescOutc1 = new sgui::TextArea(outcAreaW, outcAreaH, desc1, fontLabel, false, this);
        mDescOutc1->SetColor(colorLabel);

        const char * desc2 = temple->GetExplorationOutcomeString(o2);
        mDescOutc2 = new sgui::TextArea(outcAreaW, outcAreaH, desc2, fontLabel, false, this);
        mDescOutc2->SetColor(colorLabel);

        // GOOD OUTCOME
        if(oc == Temple::EXP_OUTC_GOOD)
        {
            textDesc->SetText(sm->GetCString("TO_DESC_GOOD"));

            mHeaderOutc1 = new graphic::Text(sm->GetCString("REWARD_1"), fontHeader);
            mHeaderOutc2 = new graphic::Text(sm->GetCString("REWARD_2"), fontHeader);

            mBtnOutcome1->SetLabel(sm->GetCString("SELECT"));
            mBtnOutcome2->SetLabel(sm->GetCString("SELECT"));
        }
        // BAD OUTCOME
        else
        {
            textDesc->SetText(sm->GetCString("TO_DESC_BAD"));

            mHeaderOutc1 = new graphic::Text(sm->GetCString("CURSE_1"), fontHeader);
            mHeaderOutc2 = new graphic::Text(sm->GetCString("CURSE_2"), fontHeader);

            mBtnOutcome1->SetLabel(sm->GetCString("SELECT"));
            mBtnOutcome2->SetLabel(sm->GetCString("SELECT"));
        }

        RegisterRenderable(mHeaderOutc1);
        RegisterRenderable(mHeaderOutc2);

        // position buttons
        const int blockW = (w - (marginSide * 2)) / 2;
        const int button1X0 = marginSide + (blockW - mBtnOutcome1->GetWidth()) / 2;
        const int buttonsY = h - marginButtonsB - mBtnOutcome1->GetHeight();

        mBtnOutcome1->SetPosition(button1X0, buttonsY);

        const int button2X0 = marginSide + blockW + (blockW - mBtnOutcome2->GetWidth()) / 2;
        mBtnOutcome2->SetPosition(button2X0, buttonsY);
    }
*/
}

void DialogExploreTempleOutcome::SetFunctionOnClose(const std::function<void()> & f)
{
    if(mBtnClose)
        mBtnClose->AddOnClickFunction(f);
}

void DialogExploreTempleOutcome::SetFunctionOnOutcome1(const std::function<void()> & f)
{
    if(mBtnOutcome1)
        mBtnOutcome1->AddOnClickFunction(f);
}

void DialogExploreTempleOutcome::SetFunctionOnOutcome2(const std::function<void()> & f)
{
    if(mBtnOutcome2)
        mBtnOutcome2->AddOnClickFunction(f);
}

void DialogExploreTempleOutcome::HandlePositionChanged()
{
    SetPositions();
}

void DialogExploreTempleOutcome::SetPositions()
{

    /*
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();
    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    const int blockW = (w - (marginSide * 2)) / 2;

    // BACKGROUND
    mBg->SetPosition(x0, y0);

    // VERTICAL LINE
    if(mLine != nullptr)
    {
        const int lineX = x0 + (w - mLine->GetWidth()) / 2;
        const int line1Y = y0 + h - marginButtonsB - mLine->GetHeight();
        mLine->SetPosition(lineX, line1Y);

        const int header1X = x0 + marginSide;
        const int headersY = y0 + 209;
        mHeaderOutc1->SetPosition(header1X, headersY);

        const int header2X = lineX + mLine->GetWidth() + marginSide;
        mHeaderOutc2->SetPosition(header2X, headersY);

        const int desc1X = header1X - x0;
        const int descY = 255;
        mDescOutc1->SetPosition(desc1X, descY);

        const int desc2X = header2X - x0;
        mDescOutc2->SetPosition(desc2X, descY);
    }
*/
}

} // namespace game
