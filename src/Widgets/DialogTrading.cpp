#include "Widgets/DialogTrading.h"

#include "Game.h"
#include "GameConstants.h"
#include "Player.h"
#include "StatValue.h"
#include "Widgets/GameButton.h"
#include "Widgets/GameUIData.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Font.h>
#include <sgl/graphic/FontManager.h>
#include <sgl/graphic/Image.h>
#include <sgl/graphic/Text.h>
#include <sgl/graphic/TextureManager.h>
#include <sgl/media/AudioManager.h>
#include <sgl/media/AudioPlayer.h>
#include <sgl/sgui/Image.h>
#include <sgl/sgui/ImageButton.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/System.h>

#include <sstream>
#include <string>

namespace game
{

constexpr int incBuy = 5;
constexpr int incSell = 5;
constexpr int digitsBuy = 3;
constexpr int digitsTot = 5;
constexpr int digitsQuantity = 4;
constexpr char digitsFill = '0';
constexpr int marginIconMoneyL = 20;
constexpr int marginIconMoneyR = 10;
constexpr int marginBtnR = 20;
constexpr int marginQuantity = 20;
constexpr int dbH = 50;
constexpr unsigned int colorData = 0xadd4ebff;

const std::string zero4("0000");

// ===== BUTTON =====
class ButtonDialogTradingClose : public sgl::sgui::ImageButton
{
public:
    ButtonDialogTradingClose(sgl::sgui::Widget * parent)
        : sgl::sgui::ImageButton({ ID_DLG_TRADING_BTN_CLOSE_NORMAL, ID_DLG_TRADING_BTN_CLOSE_NORMAL,
                                   ID_DLG_TRADING_BTN_CLOSE_OVER, ID_DLG_TRADING_BTN_CLOSE_PUSHED,
                                   ID_DLG_TRADING_BTN_CLOSE_PUSHED }, SpriteFileDialogTrading, parent)
    {
        SetShortcutKey(sgl::core::KeyboardEvent::KEY_ESCAPE);
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

class ButtonDialogTradingMinus : public sgl::sgui::ImageButton
{
public:
    ButtonDialogTradingMinus(sgl::sgui::Widget * parent)
        : sgl::sgui::ImageButton({ ID_DLG_TRADING_BTN_MINUS_NORMAL, ID_DLG_TRADING_BTN_MINUS_DISABLED,
                                  ID_DLG_TRADING_BTN_MINUS_OVER, ID_DLG_TRADING_BTN_MINUS_PUSHED,
                                  ID_DLG_TRADING_BTN_MINUS_PUSHED }, SpriteFileDialogTrading, parent)
    {
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

class ButtonDialogTradingPlus : public sgl::sgui::ImageButton
{
public:
    ButtonDialogTradingPlus(sgl::sgui::Widget * parent)
        : sgl::sgui::ImageButton({ ID_DLG_TRADING_BTN_PLUS_NORMAL, ID_DLG_TRADING_BTN_PLUS_DISABLED,
                                  ID_DLG_TRADING_BTN_PLUS_OVER, ID_DLG_TRADING_BTN_PLUS_PUSHED,
                                  ID_DLG_TRADING_BTN_PLUS_PUSHED }, SpriteFileDialogTrading, parent)
    {
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

class ButtonBuy : public GameButton
{
public:
    ButtonBuy(sgl::sgui::Widget * parent)
        : GameButton(SpriteFileDialogTrading,
                     { ID_DLG_TRADING_BTN_ACT_NORMAL, ID_DLG_TRADING_BTN_ACT_DISABLED,
                       ID_DLG_TRADING_BTN_ACT_OVER, ID_DLG_TRADING_BTN_ACT_PUSHED,
                       ID_DLG_TRADING_BTN_ACT_PUSHED }, { 0xe3e6e8ff, 0x454f54ff,
                       0xf1f2f4ff, 0xabb4baff, 0xc2c2a3ff }, parent)
    {
        using namespace sgl::graphic;

        // set label font
        auto fm = FontManager::Instance();
        Font * font = fm->GetFont("Lato-Regular.ttf", 18, Font::NORMAL);

        SetLabelFont(font);

        SetLabel("BUY");

        SetShortcutKey(sgl::core::KeyboardEvent::KEY_B);
    }
};

class ButtonSell : public GameButton
{
public:
    ButtonSell(sgl::sgui::Widget * parent)
        : GameButton(SpriteFileDialogTrading,
                     { ID_DLG_TRADING_BTN_ACT_NORMAL, ID_DLG_TRADING_BTN_ACT_DISABLED,
                       ID_DLG_TRADING_BTN_ACT_OVER, ID_DLG_TRADING_BTN_ACT_PUSHED,
                       ID_DLG_TRADING_BTN_ACT_PUSHED }, { 0xe3e6e8ff, 0x454f54ff,
                       0xf1f2f4ff, 0xabb4baff, 0xc2c2a3ff }, parent)
    {
        using namespace sgl::graphic;

        // set label font
        auto fm = FontManager::Instance();
        Font * font = fm->GetFont("Lato-Regular.ttf", 18, Font::NORMAL);

        SetLabelFont(font);

        SetLabel("SELL");

        SetShortcutKey(sgl::core::KeyboardEvent::KEY_S);
    }
};

// ===== DIALOG =====
DialogTrading::DialogTrading(Game * g, Player * p)
    : mGame(g)
    , mPlayer(p)
{
    using namespace sgl;

    // INIT DATA
    static_assert(TRADED_RES == NUM_RESOURCES, "DialogTrading - not handling all resources");

    for(unsigned int i = 0; i < NUM_RESOURCES; ++i)
    {
        mBuy[i] = 0;
        mSell[i] = 0;
    }

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();

    // BACKGROUND
    graphic::Texture * tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_BG);
    mBg = new graphic::Image(tex);
    RegisterRenderable(mBg);

    const int w = mBg->GetWidth();
    const int h = mBg->GetHeight();
    SetSize(w, h);

    // BUTTON CLOSE
    mButtonClose = new ButtonDialogTradingClose(this);

    const int buttonX = w - mButtonClose->GetWidth();
    mButtonClose->SetX(buttonX);

    // TITLE
    auto fontTitle = fm->GetFont("Lato-Regular.ttf", 32, graphic::Font::NORMAL);

    sgui::Label * title = new sgui::Label("TRADE YOUR RESOURCES", fontTitle, this);

    const int titleX = (w - title->GetWidth()) / 2;
    const int titleY = 5;
    title->SetPosition(titleX, titleY);
    title->SetColor(0xf0f3f5ff);

    // -- HEADERS --
    const int hbH = 40;
    const int hbW0 = 300;
    const int hbW1 = 200;
    const int hbW2 = 350;
    const int hbW3 = 350;
    const int marginHB = 20;
    const int hbX0 = 40;
    const int hbY0 = 75;
    const int hbX1 = hbX0 + hbW0 + marginHB;
    const int hbX2 = hbX1 + hbW1 + marginHB;
    const int hbX3 = hbX2 + hbW2 + marginHB;

    const unsigned int colorHeader = 0xd6eaf5ff;
    auto fontHeader = fm->GetFont("Lato-Regular.ttf", 22, graphic::Font::NORMAL);

    // RESOURCE
    sgui::Label * header = new sgui::Label("RESOURCE", fontHeader, this);
    header->SetColor(colorHeader);

    int headerX = hbX0 + (hbW0 - header->GetWidth()) / 2;
    int headerY = hbY0 + (hbH - header->GetHeight()) / 2;
    header->SetPosition(headerX, headerY);

    // STOCK
    header = new sgui::Label("STOCK", fontHeader, this);
    header->SetColor(colorHeader);

    headerX = hbX1 + (hbW1 - header->GetWidth()) / 2;
    headerY = hbY0 + (hbH - header->GetHeight()) / 2;
    header->SetPosition(headerX, headerY);

    // BUY
    header = new sgui::Label("BUY", fontHeader, this);
    header->SetColor(colorHeader);

    headerX = hbX2 + (hbW2 - header->GetWidth()) / 2;
    headerY = hbY0 + (hbH - header->GetHeight()) / 2;
    header->SetPosition(headerX, headerY);

    // SELL
    header = new sgui::Label("SELL", fontHeader, this);
    header->SetColor(colorHeader);

    headerX = hbX3 + (hbW3 - header->GetWidth()) / 2;
    headerY = hbY0 + (hbH - header->GetHeight()) / 2;
    header->SetPosition(headerX, headerY);

    // -- ICONS & LABELS RESOURCES --
    const int dbX0 = 40;
    const int dbY0 = 135;
    const int dbY1 = 215;
    const int dbY2 = 295;
    const int dbY3 = 375;
    const int dbY4 = 470;

    auto fontData = fm->GetFont("Lato-Regular.ttf", 20, graphic::Font::NORMAL);

    // ENERGY
    AddResBlock(dbX0, dbY0, RES_ENERGY, fontData);

    // MATERIAL
    AddResBlock(dbX0, dbY1, RES_MATERIAL1, fontData);

    // BLOBS
    AddResBlock(dbX0, dbY2, RES_BLOBS, fontData);

    // DIAMONDS
    AddResBlock(dbX0, dbY3, RES_DIAMONDS, fontData);

    // -- LABELS STOCK --
    std::ostringstream ss;

    // ENERGY
    AddStockBlock(hbX1, dbY0, hbW1, RES_ENERGY, fontData);

    // MATERIAL
    AddStockBlock(hbX1, dbY1, hbW1, RES_MATERIAL1, fontData);

    // BLOBS
    AddStockBlock(hbX1, dbY2, hbW1, RES_BLOBS, fontData);

    // DIAMONDS
    AddStockBlock(hbX1, dbY3, hbW1, RES_DIAMONDS, fontData);

    // -- BUY --
    // ENERGY
    AddBuyBlock(hbX2, dbY0, hbW2, RES_ENERGY, fontData);

    // MATERIAL
    AddBuyBlock(hbX2, dbY1, hbW2, RES_MATERIAL1, fontData);

    // BLOBS
    AddBuyBlock(hbX2, dbY2, hbW2, RES_BLOBS, fontData);

    // DIAMONDS
    AddBuyBlock(hbX2, dbY3, hbW2, RES_DIAMONDS, fontData);

    // TOTAL BUY
    tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_ICON_MONEY);
    auto icon = new sgui::Image(tex, this);

    int dataX = hbX2 + marginIconMoneyL;
    int dataY = dbY4 + (dbH - icon->GetHeight()) / 2;
    icon->SetPosition(dataX, dataY);

    mLabelTotBuy = new sgui::Label(fontData, this);
    mLabelTotBuy->SetColor(colorData);

    UpdateLabelTotalSpend();

    dataX += icon->GetWidth() + marginIconMoneyR;
    dataY = dbY4 + (dbH - mLabelTotBuy->GetHeight()) / 2;
    mLabelTotBuy->SetPosition(dataX, dataY);

    // BUTTON BUY
    const int marginBtnR = 20;

    auto btnBuy = new ButtonBuy(this);
    dataX = hbX2 + hbW2 - btnBuy->GetWidth() - marginBtnR;
    dataY = dbY4 + (dbH - btnBuy->GetHeight()) / 2;
    btnBuy->SetPosition(dataX, dataY);

    btnBuy->AddOnClickFunction([this]
    {
        Buy();
        UpdateLabelTotalSpend();
    });

    // -- SELL --
    // ENERGY
    AddSellBlock(hbX3, dbY0, hbW3, RES_ENERGY, fontData);

    // MATERIAL
    AddSellBlock(hbX3, dbY1, hbW3, RES_MATERIAL1, fontData);

    // BLOBS
    AddSellBlock(hbX3, dbY2, hbW3, RES_BLOBS, fontData);

    // DIAMONDS
    AddSellBlock(hbX3, dbY3, hbW3, RES_DIAMONDS, fontData);

    // TOTAL SELL
    tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_ICON_MONEY);
    icon = new sgui::Image(tex, this);

    dataX = hbX3 + marginIconMoneyL;
    dataY = dbY4 + (dbH - icon->GetHeight()) / 2;
    icon->SetPosition(dataX, dataY);

    mLabelTotSell = new sgui::Label(fontData, this);
    mLabelTotSell->SetColor(colorData);

    UpdateLabelTotalGain();

    dataX += icon->GetWidth() + marginIconMoneyR;
    dataY = dbY4 + (dbH - mLabelTotSell->GetHeight()) / 2;
    mLabelTotSell->SetPosition(dataX, dataY);

    // BUTTON SELL
    auto btnSell = new ButtonSell(this);
    dataX = hbX3 + hbW3 - btnSell->GetWidth() - marginBtnR;
    dataY = dbY4 + (dbH - btnSell->GetHeight()) / 2;
    btnSell->SetPosition(dataX, dataY);

    btnSell->AddOnClickFunction([this]
    {
        Sell();
        UpdateLabelTotalGain();
    });
}

void DialogTrading::SetFunctionOnClose(const std::function<void()> & f)
{
    mButtonClose->AddOnClickFunction(f);
}

void DialogTrading::AddResBlock(int x0, int y0, ResourceType res, sgl::graphic::Font * font)
{
    using namespace  sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();

    const unsigned int texIds[NUM_RESOURCES] =
    {
        ID_DLG_TRADING_ICON_ENERGY,
        ID_DLG_TRADING_ICON_MATERIAL,
        ID_DLG_TRADING_ICON_DIAMONDS,
        ID_DLG_TRADING_ICON_BLOBS
    };

    auto tex = tm->GetSprite(SpriteFileDialogTrading, texIds[res]);
    auto icon = new sgui::Image(tex, this);

    const char * text[NUM_RESOURCES] =
    {
        "ENERGY",
        "MATERIAL",
        "DIAMONDS",
        "BLOBS",
    };

    auto data = new sgui::Label(text[res], font, this);
    data->SetColor(colorData);

    const int paddingIcon = 10;
    int dataX = x0 + paddingIcon;
    int dataY = y0 + (dbH - icon->GetHeight()) / 2;
    icon->SetPosition(dataX, dataY);

    dataX += icon->GetWidth() + paddingIcon;
    dataY = y0 + (dbH - data->GetHeight()) / 2;
    data->SetPosition(dataX, dataY);
}

void DialogTrading::AddStockBlock(int x0, int y0, int bW, ResourceType res, sgl::graphic::Font * font)
{
    using namespace  sgl;

    const Player::Stat stats[NUM_RESOURCES] =
    {
        Player::ENERGY,
        Player::MATERIAL,
        Player::DIAMONDS,
        Player::BLOBS,
    };

    const StatValue & s = mPlayer->GetStat(stats[res]);

    std::ostringstream ss;
    ss << s.GetIntValue() << " / " << s.GetIntMax();

    auto label = new sgui::Label(ss.str().c_str(), font, this);
    label->SetColor(colorData);

    const int dataX = x0 + (bW - label->GetWidth()) / 2;
    const int dataY = y0 + (dbH - label->GetHeight()) / 2;
    label->SetPosition(dataX, dataY);

    mLabelStock[res] = label;
}

void DialogTrading::AddBuyBlock(int x0, int y0, int bW, ResourceType res, sgl::graphic::Font * font)
{
    using namespace  sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();

    auto tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_ICON_MONEY);
    auto icon = new sgui::Image(tex, this);

    int dataX = x0 + marginIconMoneyL;
    int dataY = y0 + (dbH - icon->GetHeight()) / 2;
    icon->SetPosition(dataX, dataY);

    std::ostringstream ss;
    ss.width(digitsBuy);
    ss.fill(digitsFill);
    ss << mGame->GetResourcePriceBuy(res);

    auto labelCost = new sgui::Label(ss.str().c_str(), font, this);
    labelCost->SetColor(colorData);

    dataX += icon->GetWidth() + marginIconMoneyR;
    dataY = y0 + (dbH - labelCost->GetHeight()) / 2;
    labelCost->SetPosition(dataX, dataY);

    auto btnPlus = new ButtonDialogTradingPlus(this);

    dataX = x0 + bW - marginBtnR - btnPlus->GetWidth();
    dataY = y0 + (dbH - btnPlus->GetHeight()) / 2;
    btnPlus->SetPosition(dataX, dataY);

    auto label = new sgui::Label(zero4.c_str(), font, this);
    label->SetColor(colorData);

    dataX -= marginQuantity + label->GetWidth();
    dataY = y0 + (dbH - label->GetHeight()) / 2;
    label->SetPosition(dataX, dataY);

    mLabelBuy[res] = label;

    auto btnMinus = new ButtonDialogTradingMinus(this);

    dataX -= marginQuantity + btnMinus->GetWidth();
    dataY = y0 + (dbH - btnMinus->GetHeight()) / 2;
    btnMinus->SetPosition(dataX, dataY);

    btnPlus->AddOnClickFunction([this, res, label]
    {
        IncBuyQuantity(res, label);

        UpdateLabelTotalSpend();
    });

    btnMinus->AddOnClickFunction([this, res, label]
    {
        DecBuyQuantity(res, label);

        UpdateLabelTotalSpend();
    });
}

void DialogTrading::AddSellBlock(int x0, int y0, int bW, ResourceType res, sgl::graphic::Font * font)
{
    using namespace  sgl;

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();

    auto tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_ICON_MONEY);
    auto icon = new sgui::Image(tex, this);

    int dataX = x0 + marginIconMoneyL;
    int dataY = y0 + (dbH - icon->GetHeight()) / 2;
    icon->SetPosition(dataX, dataY);

    std::ostringstream ss;
    ss.width(digitsBuy);
    ss.fill(digitsFill);
    ss << mGame->GetResourcePriceSell(res);

    auto labelCost = new sgui::Label(ss.str().c_str(), font, this);
    labelCost->SetColor(colorData);

    dataX += icon->GetWidth() + marginIconMoneyR;
    dataY = y0 + (dbH - labelCost->GetHeight()) / 2;
    labelCost->SetPosition(dataX, dataY);

    auto btnPlus = new ButtonDialogTradingPlus(this);

    dataX = x0 + bW - marginBtnR - btnPlus->GetWidth();
    dataY = y0 + (dbH - btnPlus->GetHeight()) / 2;
    btnPlus->SetPosition(dataX, dataY);

    auto label = new sgui::Label(zero4.c_str(), font, this);
    label->SetColor(colorData);

    dataX -= marginQuantity + label->GetWidth();
    dataY = y0 + (dbH - label->GetHeight()) / 2;
    label->SetPosition(dataX, dataY);

    mLabelSell[res] = label;

    auto btnMinus = new ButtonDialogTradingMinus(this);

    dataX -= marginQuantity + btnMinus->GetWidth();
    dataY = y0 + (dbH - btnMinus->GetHeight()) / 2;
    btnMinus->SetPosition(dataX, dataY);

    btnPlus->AddOnClickFunction([this, res, label]
    {
        IncSellQuantity(res, label);

        UpdateLabelTotalGain();
    });

    btnMinus->AddOnClickFunction([this, res, label]
    {
        DecSellQuantity(res, label);

        UpdateLabelTotalGain();
    });
}

void DialogTrading::HandlePositionChanged()
{
    SetPositions();
}

void DialogTrading::SetPositions()
{
    const int x0 = GetScreenX();
    const int y0 = GetScreenY();

    // BACKGROUND
    mBg->SetPosition(x0, y0);
}

int DialogTrading::GetCurrentSpend() const
{
    return mBuy[RES_ENERGY] * mGame->GetResourcePriceBuy(RES_ENERGY) +
        mBuy[RES_MATERIAL1] * mGame->GetResourcePriceBuy(RES_MATERIAL1) +
        mBuy[RES_BLOBS] * mGame->GetResourcePriceBuy(RES_BLOBS) +
        mBuy[RES_DIAMONDS] * mGame->GetResourcePriceBuy(RES_DIAMONDS);
}

void DialogTrading::UpdateLabelTotalSpend()
{
    const int spend = GetCurrentSpend();

    std::ostringstream ss;
    ss.width(digitsTot);
    ss.fill(digitsFill);
    ss << spend;

    mLabelTotBuy->SetText(ss.str().c_str());
}

int DialogTrading::GetCurrentGain() const
{
    return mSell[RES_ENERGY] * mGame->GetResourcePriceSell(RES_ENERGY) +
           mSell[RES_MATERIAL1] * mGame->GetResourcePriceSell(RES_MATERIAL1) +
           mSell[RES_BLOBS] * mGame->GetResourcePriceSell(RES_BLOBS) +
           mSell[RES_DIAMONDS] * mGame->GetResourcePriceSell(RES_DIAMONDS);
}

void DialogTrading::UpdateLabelTotalGain()
{
    const int spend = GetCurrentGain();

    std::ostringstream ss;
    ss.width(digitsTot);
    ss.fill(digitsFill);
    ss << spend;

    mLabelTotSell->SetText(ss.str().c_str());
}

void DialogTrading::IncBuyQuantity(ResourceType res, sgl::sgui::Label * label)
{
    const Player::Stat stats[NUM_RESOURCES] =
    {
        Player::ENERGY,
        Player::MATERIAL,
        Player::DIAMONDS,
        Player::BLOBS,
    };

    const StatValue & s = mPlayer->GetStat(stats[res]);
    const int capacity = s.GetIntMax();
    const int owned = s.GetIntValue();

    // no more capacity to buy
    if(owned == capacity)
        return ;

    const int price = mGame->GetResourcePriceBuy(res);
    const int currSpend = GetCurrentSpend();
    const int spend = currSpend + (incBuy * price);
    const int money = mPlayer->GetStat(Player::Stat::MONEY).GetIntValue();

    int inc = incBuy;

    if(spend > money)
    {
        const int units = (money - currSpend) / price;

        if(0 == units)
            return ;

        inc = units;
    }

    if(inc + owned > capacity)
        inc = capacity - owned;

    mBuy[res] += inc;

    std::ostringstream ss;
    ss.width(digitsQuantity);
    ss.fill(digitsFill);
    ss << mBuy[res];
    label->SetText(ss.str().c_str());
}

void DialogTrading::DecBuyQuantity(ResourceType res, sgl::sgui::Label * label)
{
    const int mod = mBuy[res] % incBuy;

    if(mod == 0)
    {
        if(mBuy[res] > incBuy)
            mBuy[res] -= incBuy;
        else
            mBuy[res] = 0;
    }
    else
        mBuy[res] -= mod;

    std::ostringstream ss;
    ss.width(digitsQuantity);
    ss.fill(digitsFill);
    ss << mBuy[res];
    label->SetText(ss.str().c_str());
}

void DialogTrading::IncSellQuantity(ResourceType res, sgl::sgui::Label * label)
{
    const Player::Stat stats[NUM_RESOURCES] =
    {
        Player::ENERGY,
        Player::MATERIAL,
        Player::DIAMONDS,
        Player::BLOBS,
    };

    const int owned = mPlayer->GetStat(stats[res]).GetIntValue();

    if(0 == owned)
        return ;

    if((mSell[res] + incSell) <= owned)
        mSell[res] += incSell;
    else
        mSell[res] += (owned - mSell[res]);

    std::ostringstream ss;
    ss.width(digitsQuantity);
    ss.fill(digitsFill);
    ss << mSell[res];
    label->SetText(ss.str().c_str());
}

void DialogTrading::DecSellQuantity(ResourceType res, sgl::sgui::Label * label)
{
    const int mod = mSell[res] % incSell;

    if(mod == 0)
    {
        if(mSell[res] > incSell)
            mSell[res] -= incSell;
        else
            mSell[res] = 0;
    }
    else
        mBuy[res] -= mod;

    std::ostringstream ss;
    ss.width(digitsQuantity);
    ss.fill(digitsFill);
    ss << mSell[res];
    label->SetText(ss.str().c_str());
}

void DialogTrading::Buy()
{
    // spend money
    const int spend = GetCurrentSpend();
    const int money = mPlayer->GetStat(Player::Stat::MONEY).GetIntValue();

    if(money < spend || spend == 0)
        return ;

    mPlayer->SumResource(Player::Stat::MONEY, -spend);

    // get stuff
    mPlayer->SumResource(Player::Stat::ENERGY, mBuy[RES_ENERGY]);
    mPlayer->SumResource(Player::Stat::MATERIAL, mBuy[RES_MATERIAL1]);
    mPlayer->SumResource(Player::Stat::BLOBS, mBuy[RES_BLOBS]);
    mPlayer->SumResource(Player::Stat::DIAMONDS, mBuy[RES_DIAMONDS]);

    // reset buy quantities
    for(unsigned int i = 0; i < NUM_RESOURCES; ++i)
    {
        mBuy[i] = 0;

        mLabelBuy[i]->SetText(zero4.c_str());
    }
}

void DialogTrading::Sell()
{
    // remove stuff
    const Player::Stat stats[NUM_RESOURCES] =
    {
        Player::ENERGY,
        Player::MATERIAL,
        Player::DIAMONDS,
        Player::BLOBS,
    };

    for(unsigned int i = 0; i < NUM_RESOURCES; ++i)
    {
        if(mSell[i] > mPlayer->GetStat(stats[i]).GetIntValue())
            return ;
    }

    mPlayer->SumResource(Player::Stat::ENERGY, -mSell[RES_ENERGY]);
    mPlayer->SumResource(Player::Stat::MATERIAL, -mSell[RES_MATERIAL1]);
    mPlayer->SumResource(Player::Stat::BLOBS, -mSell[RES_BLOBS]);
    mPlayer->SumResource(Player::Stat::DIAMONDS, -mSell[RES_DIAMONDS]);

    // get money
    const int gain = GetCurrentGain();

    mPlayer->SumResource(Player::Stat::MONEY, gain);

    // reset sell quantities
    for(unsigned int i = 0; i < NUM_RESOURCES; ++i)
    {
        mSell[i] = 0;

        mLabelSell[i]->SetText(zero4.c_str());
    }
}

} // namespace game
