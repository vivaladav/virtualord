#include "Widgets/DialogTrading.h"

#include "Game.h"
#include "GameConstants.h"
#include "Player.h"
#include "StatValue.h"
#include "Widgets/ButtonDialogClose.h"
#include "Widgets/GameButton.h"
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
#include <sgl/sgui/ImageButton.h>
#include <sgl/sgui/Label.h>
#include <sgl/utilities/StringManager.h>
#include <sgl/utilities/System.h>

#include <sstream>
#include <string>

// anonymous namespace for local "private" classes
namespace
{

using namespace game;

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

// ===== BUTTONS =====
class ButtonBuy : public GameButton
{
public:
    ButtonBuy(sgl::sgui::Widget * parent)
        : GameButton(SpriteFileDialogTrading,
                     { ID_DLG_TRADING_BTN_BUY_NORMAL, ID_DLG_TRADING_BTN_BUY_DISABLED,
                       ID_DLG_TRADING_BTN_BUY_OVER, ID_DLG_TRADING_BTN_BUY_PUSHED,
                       ID_DLG_TRADING_BTN_BUY_PUSHED },
                     { WidgetsConstants::colorDialogButtonOkNormal,
                       WidgetsConstants::colorDialogButtonOkDisabled,
                       WidgetsConstants::colorDialogButtonOkOver,
                       WidgetsConstants::colorDialogButtonOkPushed,
                       WidgetsConstants::colorDialogButtonOkChecked }, parent)
    {
        using namespace sgl;

        const int size = 24;

        auto fm = graphic::FontManager::Instance();
        auto sm = utilities::StringManager::Instance();

        auto fnt = fm->GetFont(WidgetsConstants::FontFileButton, size, graphic::Font::NORMAL);
        SetLabelFont(fnt);
        SetLabel(sm->GetCString("BUY"));
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

class ButtonSell : public GameButton
{
public:
    ButtonSell(sgl::sgui::Widget * parent)
        : GameButton(SpriteFileDialogTrading,
                     { ID_DLG_TRADING_BTN_SELL_NORMAL, ID_DLG_TRADING_BTN_SELL_DISABLED,
                      ID_DLG_TRADING_BTN_SELL_OVER, ID_DLG_TRADING_BTN_SELL_PUSHED,
                      ID_DLG_TRADING_BTN_SELL_PUSHED },
                     { WidgetsConstants::colorDialogButtonNoNormal,
                       WidgetsConstants::colorDialogButtonNoDisabled,
                       WidgetsConstants::colorDialogButtonNoOver,
                       WidgetsConstants::colorDialogButtonNoPushed,
                       WidgetsConstants::colorDialogButtonNoChecked }, parent)
    {
        using namespace sgl;

        const int size = 24;

        auto fm = graphic::FontManager::Instance();
        auto sm = utilities::StringManager::Instance();

        auto fnt = fm->GetFont(WidgetsConstants::FontFileButton, size, graphic::Font::NORMAL);
        SetLabelFont(fnt);
        SetLabel(sm->GetCString("SELL"));
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
        : sgl::sgui::ImageButton({ ID_DLG_TRADING_BTN_MINUS_NORMAL,
                                   ID_DLG_TRADING_BTN_MINUS_DISABLED, ID_DLG_TRADING_BTN_MINUS_OVER,
                                   ID_DLG_TRADING_BTN_MINUS_PUSHED, ID_DLG_TRADING_BTN_MINUS_PUSHED },
                                 SpriteFileDialogTrading, parent)
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


} // namespace

namespace game
{

// ===== DIALOG =====
DialogTrading::DialogTrading(Game * g, Player * p)
    : mGame(g)
    , mPlayer(p)
{
    using namespace sgl;

    // INIT DATA
    // static_assert(TRADED_RES == NUM_RESOURCES, "DialogTrading - not handling all resources");

    for(unsigned int i = 0; i < NUM_RESOURCES; ++i)
    {
        mBuy[i] = 0;
        mSell[i] = 0;
    }

    auto fm = graphic::FontManager::Instance();
    auto tm = graphic::TextureManager::Instance();
    auto sm = utilities::StringManager::Instance();


    // BACKGROUND
    const int w = 1440;
    graphic::Texture * tex;

    tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_BG_L);
    mBgL = new graphic::Image(tex);
    RegisterRenderable(mBgL);

    const int wL = mBgL->GetWidth();
    const int h = mBgL->GetHeight();

    tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_BG_R);
    mBgR = new graphic::Image(tex);
    RegisterRenderable(mBgR);

    const int wR = mBgR->GetWidth();

    tex = tm->GetSprite(SpriteFileDialogTradingExp, ID_DLG_TRADING_BG_C);
    tex->SetScaleMode(graphic::TSCALE_NEAREST);
    mBgC = new graphic::Image(tex);
    RegisterRenderable(mBgC);

    const int wC = w - wL - wR;
    mBgC->SetWidth(wC);

    SetSize(w, h);

    // BUTTON CLOSE
    mButtonClose = new ButtonDialogClose(this);

    const int buttonX = w - mButtonClose->GetWidth();
    mButtonClose->SetX(buttonX);

    // TITLE
    auto font = fm->GetFont(WidgetsConstants::FontFileDialogTitle,
                            WidgetsConstants::FontSizeDialogTitle, graphic::Font::NORMAL);
    auto title = new sgui::Label(sm->GetCString("TRADE_UR_RES"), font, this);
    title->SetColor(WidgetsConstants::colorDialogTitle);

    const int titleY = (WidgetsConstants::DialogTitleBarH - title->GetHeight()) / 2;
    title->SetPosition(WidgetsConstants::MarginDialogTitleL, titleY);

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
    auto fontHeader = fm->GetFont(WidgetsConstants::FontFileHeader, 22, graphic::Font::NORMAL);

    // RESOURCE
    sgui::Label * header = new sgui::Label(sm->GetCString("RESOURCE"), fontHeader, this);
    header->SetColor(colorHeader);

    int headerX = hbX0 + (hbW0 - header->GetWidth()) / 2;
    int headerY = hbY0 + (hbH - header->GetHeight()) / 2;
    header->SetPosition(headerX, headerY);

    // STOCK
    header = new sgui::Label(sm->GetCString("STOCK"), fontHeader, this);
    header->SetColor(colorHeader);

    headerX = hbX1 + (hbW1 - header->GetWidth()) / 2;
    headerY = hbY0 + (hbH - header->GetHeight()) / 2;
    header->SetPosition(headerX, headerY);

    // BUY
    header = new sgui::Label(sm->GetCString("BUY"), fontHeader, this);
    header->SetColor(colorHeader);

    headerX = hbX2 + (hbW2 - header->GetWidth()) / 2;
    headerY = hbY0 + (hbH - header->GetHeight()) / 2;
    header->SetPosition(headerX, headerY);

    // SELL
    header = new sgui::Label(sm->GetCString("SELL"), fontHeader, this);
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

    auto fontData = fm->GetFont(WidgetsConstants::FontFileText, 20, graphic::Font::NORMAL);

    mCallbackValIds.assign(NUM_RESOURCES, RES_INVALID);

    // ENERGY
    AddResBlock(dbX0, dbY0, RES_ENERGY, fontData);

    auto st = Player::Stat::ENERGY;
    mCallbackValIds[st] = p->AddOnResourceChanged(st, [this](const StatValue * val, int, int)
    {
        UpdateStockLabel(val->GetId());
    });

    // MATERIAL
    AddResBlock(dbX0, dbY1, RES_MATERIAL1, fontData);

    st = Player::Stat::MATERIAL;
    mCallbackValIds[st] = p->AddOnResourceChanged(st, [this](const StatValue * val, int, int)
    {
        UpdateStockLabel(val->GetId());
    });

    // BLOBS
    AddResBlock(dbX0, dbY2, RES_BLOBS, fontData);

    st = Player::Stat::BLOBS;
    mCallbackValIds[st] = p->AddOnResourceChanged(st, [this](const StatValue * val, int, int)
    {
        UpdateStockLabel(val->GetId());
    });

    // DIAMONDS
    AddResBlock(dbX0, dbY3, RES_DIAMONDS, fontData);

    st = Player::Stat::DIAMONDS;
    mCallbackValIds[st] = p->AddOnResourceChanged(st, [this](const StatValue * val, int, int)
    {
        UpdateStockLabel(val->GetId());
    });

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
    tex = tm->GetSprite(SpriteFileGameUIShared, ID_UIS_ICON_C_RES_MONEY_24);
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
    tex = tm->GetSprite(SpriteFileGameUIShared, ID_UIS_ICON_C_RES_MONEY_24);
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

DialogTrading::~DialogTrading()
{
    // CLEAR OBSERVERS FROM PLAYER STATS
    auto st = Player::Stat::MONEY;
    mPlayer->RemoveOnResourceChanged(st, mCallbackValIds[st]);

    st = Player::Stat::ENERGY;
    mPlayer->RemoveOnResourceChanged(st, mCallbackValIds[st]);

    st = Player::Stat::MATERIAL;
    mPlayer->RemoveOnResourceChanged(st, mCallbackValIds[st]);

    st = Player::Stat::DIAMONDS;
    mPlayer->RemoveOnResourceChanged(st, mCallbackValIds[st]);

    st = Player::Stat::BLOBS;
    mPlayer->RemoveOnResourceChanged(st, mCallbackValIds[st]);
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
    auto sm = utilities::StringManager::Instance();

    const unsigned int texIds[] =
    {
        ID_UIS_ICON_C_RES_ENERGY_24,
        ID_UIS_ICON_C_RES_MATERIAL_24,
        ID_UIS_ICON_C_RES_DIAMONDS_24,
        ID_UIS_ICON_C_RES_BLOBS_24,
        ID_UIS_ICON_C_RES_RESEARCH_24,
    };

    auto tex = tm->GetSprite(SpriteFileGameUIShared, texIds[res]);
    auto icon = new sgui::Image(tex, this);

    const char * text[] =
    {
        sm->GetCString("ENERGY"),
        sm->GetCString("MATERIAL"),
        sm->GetCString("DIAMONDS"),
        sm->GetCString("BLOBS"),
        sm->GetCString("RESEARCH"),
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

    const Player::Stat stats[] =
    {
        Player::ENERGY,
        Player::MATERIAL,
        Player::DIAMONDS,
        Player::BLOBS,
        Player::RESEARCH,
    };

    const StatValue & s = mPlayer->GetStat(stats[res]);

    std::ostringstream ss;
    ss << s.GetValue() << " / " << s.GetMax();

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

    auto tex = tm->GetSprite(SpriteFileGameUIShared, ID_UIS_ICON_C_RES_MONEY_24);
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

    auto tex = tm->GetSprite(SpriteFileGameUIShared, ID_UIS_ICON_C_RES_MONEY_24);
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
    const int y = GetScreenY();
    int x = GetScreenX();

    mBgL->SetPosition(x, y);
    x += mBgL->GetWidth();

    mBgC->SetPosition(x, y);
    x += mBgC->GetWidth();

    mBgR->SetPosition(x, y);
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
    const int capacity = s.GetMax();
    const int owned = s.GetValue();

    // no more capacity to buy
    if(owned == capacity)
        return ;

    const int price = mGame->GetResourcePriceBuy(res);
    const int currSpend = GetCurrentSpend();
    const int spend = currSpend + (incBuy * price);
    const int money = mPlayer->GetStat(Player::Stat::MONEY).GetValue();

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

    const int owned = mPlayer->GetStat(stats[res]).GetValue();

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

void DialogTrading::UpdateStockLabel(unsigned int statId)
{
    using namespace  sgl;

    const ResourceType resources[NUM_RESOURCES] =
    {
        RES_BLOBS,
        RES_DIAMONDS,
        RES_ENERGY,
        RES_MATERIAL1
    };

    const ResourceType res = resources[statId];
    const StatValue & s = mPlayer->GetStat(static_cast<Player::Stat>(statId));

    auto label = mLabelStock[res];

    const int oldX = label->GetX();
    const int oldY = label->GetY();
    const int oldW = label->GetWidth();
    const int oldH = label->GetHeight();

    // update text
    std::ostringstream ss;
    ss << s.GetValue() << " / " << s.GetMax();

    label->SetText(ss.str().c_str());

    const int newW = label->GetWidth();
    const int newH = label->GetHeight();

    // reposition
    const int dataX = oldX + (oldW - newW) / 2;
    const int dataY = oldY + (oldH - newH) / 2;
    label->SetPosition(dataX, dataY);
}

void DialogTrading::Buy()
{
    // spend money
    const int spend = GetCurrentSpend();
    const int money = mPlayer->GetStat(Player::Stat::MONEY).GetValue();

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
        if(mSell[i] > mPlayer->GetStat(stats[i]).GetValue())
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
