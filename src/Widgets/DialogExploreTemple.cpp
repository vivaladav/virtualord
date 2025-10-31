#include "Widgets/DialogExploreTemple.h"

#include "Game.h"
#include "Player.h"
#include "GameObjects/Temple.h"
#include "Widgets/GameButton.h"
#include "Widgets/GameSliderH.h"
#include "Widgets/GameUIData.h"

#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/Label.h>
#include <sgl/sgui/TextArea.h>

#include <sstream>

namespace game
{

// ===== BUTTON =====
class ButtonExploreTemple : public GameButton
{
public:
    ButtonExploreTemple(sgl::sgui::Widget * parent)
        : GameButton(SpriteFileDialogExploreTemple,
        { ID_DLG_EXTM_BTN_NORMAL, ID_DLG_EXTM_BTN_DISABLED,
          ID_DLG_EXTM_BTN_OVER, ID_DLG_EXTM_BTN_PUSHED, ID_DLG_EXTM_BTN_PUSHED },
        { 0xc3dfeeff, 0x5a6266ff, 0xebf4f9ff, 0xc3dfeeff, 0xc3dfeeff },
        parent)
    {
        using namespace sgl;

        const char * fileFont = "Lato-Regular.ttf";
        const int size = 22;

        auto fm = graphic::FontManager::Instance();
        graphic::Font * fnt = fm->GetFont(fileFont, size, graphic::Font::NORMAL);
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

// ===== DIALOG EXPLORE TEMPLE =====
const int marginSide = 40;
const int marginButtonsB = 20;

DialogExploreTemple::DialogExploreTemple(Player * player, Temple * temple)
    : mPlayer(player)
    , mTemple(temple)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();

    const char * headerFontFile = "Lato-Regular.ttf";
    const int headerFontSize = 22;
    const char * labelFontFile = "Lato-Regular.ttf";
    const unsigned int colorHeader = 0x9dcbe2ff;
    const unsigned int colorLabel = 0x70a7c2ff;

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFileDialogExploreTemple, ID_DLG_EXTM_BACKGROUND);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    SetSize(w, h);

    // TITLE
    auto fontTitle = fm->GetFont("Lato-Regular.ttf", 32, graphic::Font::NORMAL);

    sgui::Label * title = new sgui::Label("EXPLORE ABANDONED TEMPLE", fontTitle, this);

    const int titleX = (w - title->GetWidth()) / 2;
    const int titleY = 10;
    title->SetPosition(titleX, titleY);
    title->SetColor(0xf1f3f4ff);

    // HORIZ BAR 1
    tex = tm->GetSprite(SpriteFileDialogExploreTemple, ID_DLG_EXTM_LINE_H);
    mLineH1 = new graphic::Image(tex);
    RegisterRenderable(mLineH1);

    // HORIZ BAR 2
    mLineH2 = new graphic::Image(tex);
    RegisterRenderable(mLineH2);

    // -- INVESTING PANEL --
    const int sliderLabelFontSize = 18;
    const int iconsX0 = marginSide;
    const int iconsY0 = 135;
    const int iconsX1 = 510;
    const int iconsY1 = 215;
    const int marginIconR = 20;
    const int marginSliderR = 20;

    const int minInvest = 0;

    graphic::Texture * texSliderBg = tm->GetSprite(SpriteFileDialogExploreTemple, ID_DLG_EXTM_SLIDER_BG);
    graphic::Texture * texSliderBar = tm->GetSprite(SpriteFileDialogExploreTemple, ID_DLG_EXTM_SLIDER_BAR);
    graphic::Texture * texSliderBtn = tm->GetSprite(SpriteFileDialogExploreTemple, ID_DLG_EXTM_SLIDER_BTN);

    auto fontHeader = fm->GetFont(headerFontFile, headerFontSize, graphic::Font::NORMAL);
    auto fontLabel = fm->GetFont(labelFontFile, sliderLabelFontSize, graphic::Font::NORMAL);

    // HEADER INVEST
    mHeaderInvest = new graphic::Text("INVEST RESOURCES", fontHeader);
    mHeaderInvest->SetColor(colorHeader);
    RegisterRenderable(mHeaderInvest);

    // MONEY
    const int playerMoney = player->GetStat(Player::Stat::MONEY).GetValue();
    const int maxTempleMoney = temple->GetMaxInvestableMoney();
    const int maxMoney =  playerMoney < maxTempleMoney ? playerMoney : maxTempleMoney;

    tex = tm->GetSprite(SpriteFileDialogExploreTemple, ID_DLG_EXTM_ICON_MONEY);

    auto icon = new sgui::Image(tex, this);
    icon->SetColor(colorHeader);
    icon->SetPosition(iconsX0, iconsY0);

    mSliderMoney = new GameSliderH(texSliderBg, texSliderBar, texSliderBtn, this);
    mSliderMoney->SetMinMax(minInvest, maxMoney);
    mSliderMoney->SetValue(minInvest + (maxMoney - minInvest) / 2);
    mSliderMoney->SetPosition(icon->GetX() + icon->GetWidth() + marginIconR,
                              icon->GetY() + (icon->GetHeight() - mSliderMoney->GetHeight()) / 2);

    auto label = new sgui::Label(std::to_string(mSliderMoney->GetValue()).c_str(), fontLabel, this);
    label->SetColor(colorLabel);
    label->SetPosition(mSliderMoney->GetX() + mSliderMoney->GetWidth() + marginSliderR,
                       mSliderMoney->GetY());

    mSliderMoney->SetOnValueChanged([this, label](int val)
    {
        label->SetText(std::to_string(val).c_str());

        OnInvestmentChanged();
    });

    // MATERIAL
    const int playerMaterial = player->GetStat(Player::Stat::MATERIAL).GetValue();
    const int maxTempleMaterial = temple->GetMaxInvestableMaterial();
    const int maxMaterial =  playerMaterial < maxTempleMaterial ? playerMaterial : maxTempleMaterial;

    tex = tm->GetSprite(SpriteFileDialogExploreTemple, ID_DLG_EXTM_ICON_MATERIAL);

    icon = new sgui::Image(tex, this);
    icon->SetColor(colorHeader);
    icon->SetPosition(iconsX0, iconsY1);

    mSliderMaterial = new GameSliderH(texSliderBg, texSliderBar, texSliderBtn, this);
    mSliderMaterial->SetMinMax(minInvest, maxMaterial);
    mSliderMaterial->SetValue(minInvest + (maxMaterial - minInvest) / 2);
    mSliderMaterial->SetPosition(icon->GetX() + icon->GetWidth() + marginIconR,
                                 icon->GetY() + (icon->GetHeight() - mSliderMoney->GetHeight()) / 2);

    label = new sgui::Label(std::to_string(mSliderMaterial->GetValue()).c_str(), fontLabel, this);
    label->SetColor(colorLabel);
    label->SetPosition(mSliderMaterial->GetX() + mSliderMaterial->GetWidth() + marginSliderR,
                       mSliderMaterial->GetY());

    mSliderMaterial->SetOnValueChanged([this, label](int val)
    {
        label->SetText(std::to_string(val).c_str());

        OnInvestmentChanged();
    });

    // BLOBS
    const int playerBlobs = player->GetStat(Player::Stat::BLOBS).GetValue();
    const int maxTempleBlobs = temple->GetMaxInvestableBlobs();
    const int maxBlobs =  playerBlobs < maxTempleBlobs ? playerBlobs : maxTempleBlobs;

    tex = tm->GetSprite(SpriteFileDialogExploreTemple, ID_DLG_EXTM_ICON_BLOBS);

    icon = new sgui::Image(tex, this);
    icon->SetColor(colorHeader);
    icon->SetPosition(iconsX1, iconsY0);

    mSliderBlobs = new GameSliderH(texSliderBg, texSliderBar, texSliderBtn, this);
    mSliderBlobs->SetMinMax(minInvest, maxBlobs);
    mSliderBlobs->SetValue(minInvest + (maxBlobs - minInvest) / 2);
    mSliderBlobs->SetPosition(icon->GetX() + icon->GetWidth() + marginIconR,
                              icon->GetY() + (icon->GetHeight() - mSliderBlobs->GetHeight()) / 2);

    label = new sgui::Label(std::to_string(mSliderBlobs->GetValue()).c_str(), fontLabel, this);
    label->SetColor(colorLabel);
    label->SetPosition(mSliderBlobs->GetX() + mSliderBlobs->GetWidth() + marginSliderR,
                       mSliderBlobs->GetY());

    mSliderBlobs->SetOnValueChanged([this, label](int val)
    {
        label->SetText(std::to_string(val).c_str());

        OnInvestmentChanged();
    });

    // DIAMONDS
    const int playerDiamonds = player->GetStat(Player::Stat::DIAMONDS).GetValue();
    const int maxTempleDiamonds = temple->GetMaxInvestableBlobs();
    const int maxDiamonds =  playerDiamonds < maxTempleDiamonds ? playerDiamonds : maxTempleDiamonds;

    tex = tm->GetSprite(SpriteFileDialogExploreTemple, ID_DLG_EXTM_ICON_DIAMONDS);

    icon = new sgui::Image(tex, this);
    icon->SetColor(colorHeader);
    icon->SetPosition(iconsX1, iconsY1);

    mSliderDiamonds = new GameSliderH(texSliderBg, texSliderBar, texSliderBtn, this);
    mSliderDiamonds->SetMinMax(minInvest, maxDiamonds);
    mSliderDiamonds->SetValue(minInvest + (maxBlobs - minInvest) / 2);
    mSliderDiamonds->SetPosition(icon->GetX() + icon->GetWidth() + marginIconR,
                                 icon->GetY() + (icon->GetHeight() - mSliderDiamonds->GetHeight()) / 2);

    label = new sgui::Label(std::to_string(mSliderDiamonds->GetValue()).c_str(), fontLabel, this);
    label->SetColor(colorLabel);
    label->SetPosition(mSliderDiamonds->GetX() + mSliderDiamonds->GetWidth() + marginSliderR,
                       mSliderDiamonds->GetY());

    mSliderDiamonds->SetOnValueChanged([this, label](int val)
    {
        label->SetText(std::to_string(val).c_str());

        OnInvestmentChanged();
    });

    // -- OUTCOME PANEL --
    mHeaderTurns = new graphic::Text("TURNS REQUIRED", fontHeader);
    mHeaderTurns->SetColor(colorHeader);
    RegisterRenderable(mHeaderTurns);

    mLabelTurns = new sgui::Label(fontLabel, this);
    mLabelTurns->SetColor(colorLabel);

    mHeaderSuccess = new graphic::Text("SUCCESS PROBABILITY", fontHeader);
    mHeaderSuccess->SetColor(colorHeader);
    RegisterRenderable(mHeaderSuccess);

    mLabelSuccess = new sgui::Label(fontLabel, this);
    mLabelSuccess->SetColor(colorLabel);

    // -- BUTTONS --
    const int blockW = (w - (marginSide * 2)) / 2;

    // BUTTON ABORT
    mBtnAbort = new ButtonExploreTemple(this);
    mBtnAbort->SetLabel("ABORT");

    // BUTTON EXPLORE
    mBtnExplore = new ButtonExploreTemple(this);
    mBtnExplore->SetLabel("EXPLORE");

    // position buttons
    const int button1X0 = marginSide + (blockW - mBtnAbort->GetWidth()) / 2;
    const int buttonsY = h - marginButtonsB - mBtnAbort->GetHeight();

    mBtnAbort->SetPosition(button1X0, buttonsY);

    const int button2X0 = marginSide + blockW + (blockW - mBtnExplore->GetWidth()) / 2;
    mBtnExplore->SetPosition(button2X0, buttonsY);

    // INIT VALUES
    OnInvestmentChanged();
}

void DialogExploreTemple::SetFunctionOnCancel(const std::function<void()> & f)
{
    mBtnAbort->AddOnClickFunction(f);
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
    // update invested resources
    mTemple->SetInvestedResources(mSliderMoney->GetValue(), mSliderMaterial->GetValue(),
                                  mSliderBlobs->GetValue(), mSliderDiamonds->GetValue());

    // update turns
    const int turns = mTemple->GetExplorationTurns();

    mLabelTurns->SetText(std::to_string(turns).c_str());

    // update success rate
    const int success = mTemple->GetExplorationSuccessRate();

    std::ostringstream ss;
    ss << success << "%";

    mLabelSuccess->SetText(ss.str().c_str());

    // reposition elements
    SetPositions();
}

void DialogExploreTemple::SetPositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();
    const int w = mBg->GetWidth();
    const int blockW = (w - (marginSide * 2)) / 2;

    // BACKGROUND
    mBg->SetPosition(x0, y0);

    // HORIZ LINE 1
    const int lineX = x0 + (w - mLineH1->GetWidth()) / 2;
    const int line1Y = y0 + 280;
    mLineH1->SetPosition(lineX, line1Y);

    // HORIZ LINE 2
    const int line2Y = y0 + 410;
    mLineH2->SetPosition(lineX, line2Y);

    // -- INVESTING PANEL --
    const int marginPanel1V = 85;

    // HEADER INVEST
    const int headerInvestX = x0 + marginSide;
    const int headerInvestY = y0 + marginPanel1V;
    mHeaderInvest->SetPosition(headerInvestX, headerInvestY);

    // -- OUTCOME PANEL --
    const int marginPanel2V = 40;
    const int marginHeaderB = 5;

    // HEADER TURNS
    const int headerTurnsX = x0 + marginSide + (blockW - mHeaderTurns->GetWidth()) / 2;
    const int headerTurnsY = line1Y + marginPanel2V;
    mHeaderTurns->SetPosition(headerTurnsX, headerTurnsY);

    // LABEL TURNS
    const int labelTurnsX = headerTurnsX + (mHeaderTurns->GetWidth() - mLabelTurns->GetWidth()) / 2 - x0;
    const int labelTurnsY = headerTurnsY + mHeaderTurns->GetHeight() + marginHeaderB - y0;
    mLabelTurns->SetPosition(labelTurnsX, labelTurnsY);

    // HEADER SUCCESS
    const int headerSuccessX = x0 + marginSide + blockW + (blockW - mHeaderSuccess->GetWidth()) / 2;
    mHeaderSuccess->SetPosition(headerSuccessX, headerTurnsY);

    // LABEL SUCCESS
    const int labelSuccessX = headerSuccessX + (mHeaderSuccess->GetWidth() - mLabelSuccess->GetWidth()) / 2  - x0;
    mLabelSuccess->SetPosition(labelSuccessX, labelTurnsY);
}

// ===== DIALOG EXPLORE TEMPLE OUTCOME =====
DialogExploreTempleOutcome::DialogExploreTempleOutcome(Player * player, Temple * temple)
    : mPlayer(player)
    , mTemple(temple)
{
    using namespace sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();

    const char * headerFontFile = "Lato-Regular.ttf";
    const int headerFontSize = 22;
    const char * labelFontFile = "Lato-Regular.ttf";
    const int labelFontSize = 22;
    const unsigned int colorHeader = 0x9dcbe2ff;
    const unsigned int colorLabel = 0x70a7c2ff;

    const int marginPanel1V = 85;

    auto fontLabel = fm->GetFont(labelFontFile, labelFontSize, graphic::Font::NORMAL);

    const Temple::ExplorationOutcomeCategory oc = temple->GetExplorationOutcomeCategory();

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFileDialogExploreTemple, ID_DLG_EXTM_BACKGROUND);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    SetSize(w, h);

    // TITLE
    auto fontTitle = fm->GetFont("Lato-Regular.ttf", 32, graphic::Font::NORMAL);

    sgui::Label * title = new sgui::Label("TEMPLE EXPLORATION OUTCOME", fontTitle, this);

    const int titleX = (w - title->GetWidth()) / 2;
    const int titleY = 10;
    title->SetPosition(titleX, titleY);
    title->SetColor(0xf1f3f4ff);

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
        const char * text = "The exploration failed, but at least nothing bad happened!\n\n"
                            "You can try again, good luck.";
        textDesc->SetText(text);

        mBtnClose = new ButtonExploreTemple(this);
        mBtnClose->SetLabel("LEAVE");

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
        auto fontHeader = fm->GetFont(headerFontFile, headerFontSize, graphic::Font::NORMAL);

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
            const char * text = "The exploration revealed a powerful talisman that can be used to "
                                "help your mission.\n"
                                "Pick your reward between these 2.";
            textDesc->SetText(text);

            mHeaderOutc1 = new graphic::Text("REWARD 1", fontHeader);
            mHeaderOutc2 = new graphic::Text("REWARD 2", fontHeader);

            mBtnOutcome1->SetLabel("REWARD 1");
            mBtnOutcome2->SetLabel("REWARD 2");
        }
        // BAD OUTCOME
        else
        {
            const char * text = "The exploration failed and unlocked a powerful curse that will "
                                "punish your attempt.\n"
                                "Pick your punishment between these 2.";
            textDesc->SetText(text);

            mHeaderOutc1 = new graphic::Text("CURSE 1", fontHeader);
            mHeaderOutc2 = new graphic::Text("CURSE 2", fontHeader);

            mBtnOutcome1->SetLabel("CURSE 1");
            mBtnOutcome2->SetLabel("CURSE 2");
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
}

} // namespace game
