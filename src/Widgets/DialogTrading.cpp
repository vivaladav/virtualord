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

// ===== DIALOG =====
DialogTrading::DialogTrading(Game * g, Player * p)
    : mGame(g)
    , mPlayer(p)
{
    using namespace sgl;

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
    const int dbH = 50;
    const int paddingIcon = 10;

    const unsigned int colorData = 0xadd4ebff;
    auto fontData = fm->GetFont("Lato-Regular.ttf", 20, graphic::Font::NORMAL);

    // ENERGY
    tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_ICON_ENERGY);
    auto icon = new sgui::Image(tex, this);

    auto data = new sgui::Label("ENERGY", fontData, this);
    data->SetColor(colorData);

    int dataX = dbX0 + paddingIcon;
    int dataY = dbY0 + (dbH - icon->GetHeight()) / 2;
    icon->SetPosition(dataX, dataY);

    dataX += icon->GetWidth() + paddingIcon;
    dataY = dbY0 + (dbH - data->GetHeight()) / 2;
    data->SetPosition(dataX, dataY);

    // MATERIAL
    tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_ICON_MATERIAL);
    icon = new sgui::Image(tex, this);

    data = new sgui::Label("MATERIAL", fontData, this);
    data->SetColor(colorData);

    dataX = dbX0 + paddingIcon;
    dataY = dbY1 + (dbH - icon->GetHeight()) / 2;
    icon->SetPosition(dataX, dataY);

    dataX += icon->GetWidth() + paddingIcon;
    dataY = dbY1 + (dbH - data->GetHeight()) / 2;
    data->SetPosition(dataX, dataY);

    // BLOBS
    tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_ICON_BLOBS);
    icon = new sgui::Image(tex, this);

    data = new sgui::Label("BLOBS", fontData, this);
    data->SetColor(colorData);

    dataX = dbX0 + paddingIcon;
    dataY = dbY2 + (dbH - icon->GetHeight()) / 2;
    icon->SetPosition(dataX, dataY);

    dataX += icon->GetWidth() + paddingIcon;
    dataY = dbY2 + (dbH - data->GetHeight()) / 2;
    data->SetPosition(dataX, dataY);

    // DIAMONDS
    tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_ICON_DIAMONDS);
    icon = new sgui::Image(tex, this);

    data = new sgui::Label("DIAMONDS", fontData, this);
    data->SetColor(colorData);

    dataX = dbX0 + paddingIcon;
    dataY = dbY3 + (dbH - icon->GetHeight()) / 2;
    icon->SetPosition(dataX, dataY);

    dataX += icon->GetWidth() + paddingIcon;
    dataY = dbY3 + (dbH - data->GetHeight()) / 2;
    data->SetPosition(dataX, dataY);

    // -- LABELS STOCK --
    std::ostringstream ss;

    // ENERGY
    const StatValue & s0 = mPlayer->GetStat(Player::ENERGY);

    ss << s0.GetIntValue() << " / " << s0.GetIntMax();

    mLabelStockEnergy = new sgui::Label(ss.str().c_str(), fontData, this);
    mLabelStockEnergy->SetColor(colorData);

    dataX = hbX1 + (hbW1 - mLabelStockEnergy->GetWidth()) / 2;
    dataY = dbY0 + (dbH - mLabelStockEnergy->GetHeight()) / 2;
    mLabelStockEnergy->SetPosition(dataX, dataY);

    // MATERIAL
    const StatValue & s1 = mPlayer->GetStat(Player::MATERIAL);

    ss.str(std::string());
    ss.clear();
    ss << s1.GetIntValue() << " / " << s1.GetIntMax();

    mLabelStockMaterial = new sgui::Label(ss.str().c_str(), fontData, this);
    mLabelStockMaterial->SetColor(colorData);

    dataX = hbX1 + (hbW1 - mLabelStockMaterial->GetWidth()) / 2;
    dataY = dbY1 + (dbH - mLabelStockMaterial->GetHeight()) / 2;
    mLabelStockMaterial->SetPosition(dataX, dataY);

    // BLOBS
    const StatValue & s2 = mPlayer->GetStat(Player::BLOBS);

    ss.str(std::string());
    ss.clear();
    ss << s2.GetIntValue() << " / " << s2.GetIntMax();

    mLabelStockBlobs = new sgui::Label(ss.str().c_str(), fontData, this);
    mLabelStockBlobs->SetColor(colorData);

    dataX = hbX1 + (hbW1 - mLabelStockBlobs->GetWidth()) / 2;
    dataY = dbY2 + (dbH - mLabelStockBlobs->GetHeight()) / 2;
    mLabelStockBlobs->SetPosition(dataX, dataY);

    // DIAMONDS
    const StatValue & s3 = mPlayer->GetStat(Player::DIAMONDS);

    ss.str(std::string());
    ss.clear();
    ss << s3.GetIntValue() << " / " << s3.GetIntMax();

    mLabelStockDiamonds = new sgui::Label(ss.str().c_str(), fontData, this);
    mLabelStockDiamonds->SetColor(colorData);

    dataX = hbX1 + (hbW1 - mLabelStockDiamonds->GetWidth()) / 2;
    dataY = dbY3 + (dbH - mLabelStockDiamonds->GetHeight()) / 2;
    mLabelStockDiamonds->SetPosition(dataX, dataY);

    // -- LABELS TOTAL BUY --
    const int marginIconMoneyL = 20;
    const int marginIconMoneyR = 10;

    const int digitsBuy = 3;
    const int digitsTot = 5;
    const char digitsFill = '0';

    const std::string zero("0");
    ss.str(std::string());
    ss.clear();

    // ENERGY
    tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_ICON_MONEY);
    icon = new sgui::Image(tex, this);

    dataX = hbX2 + marginIconMoneyL;
    dataY = dbY0 + (dbH - icon->GetHeight()) / 2;
    icon->SetPosition(dataX, dataY);

    ss.width(digitsBuy);
    ss.fill(digitsFill);
    ss << g->GetResourcePriceBuy(RES_ENERGY);

    mLabelTotBuyEnergy = new sgui::Label(ss.str().c_str(), fontData, this);
    mLabelTotBuyEnergy->SetColor(colorData);

    dataX += icon->GetWidth() + marginIconMoneyR;
    dataY = dbY0 + (dbH - mLabelTotBuyEnergy->GetHeight()) / 2;
    mLabelTotBuyEnergy->SetPosition(dataX, dataY);

    // MATERIAL
    tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_ICON_MONEY);
    icon = new sgui::Image(tex, this);

    dataX = hbX2 + marginIconMoneyL;
    dataY = dbY1 + (dbH - icon->GetHeight()) / 2;
    icon->SetPosition(dataX, dataY);

    ss.str(std::string());
    ss.clear();
    ss.width(digitsBuy);
    ss.fill(digitsFill);
    ss << g->GetResourcePriceBuy(RES_MATERIAL1);

    mLabelTotBuyMaterial = new sgui::Label(ss.str().c_str(), fontData, this);
    mLabelTotBuyMaterial->SetColor(colorData);

    dataX += icon->GetWidth() + marginIconMoneyR;
    dataY = dbY1 + (dbH - mLabelTotBuyMaterial->GetHeight()) / 2;
    mLabelTotBuyMaterial->SetPosition(dataX, dataY);

    // BLOBS
    tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_ICON_MONEY);
    icon = new sgui::Image(tex, this);

    dataX = hbX2 + marginIconMoneyL;
    dataY = dbY2 + (dbH - icon->GetHeight()) / 2;
    icon->SetPosition(dataX, dataY);

    ss.str(std::string());
    ss.clear();
    ss.width(digitsBuy);
    ss.fill(digitsFill);
    ss << g->GetResourcePriceBuy(RES_BLOBS);

    mLabelTotBuyBlobs = new sgui::Label(ss.str().c_str(), fontData, this);
    mLabelTotBuyBlobs->SetColor(colorData);

    dataX += icon->GetWidth() + marginIconMoneyR;
    dataY = dbY2 + (dbH - mLabelTotBuyBlobs->GetHeight()) / 2;
    mLabelTotBuyBlobs->SetPosition(dataX, dataY);

    // DIAMONDS
    tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_ICON_MONEY);
    icon = new sgui::Image(tex, this);

    dataX = hbX2 + marginIconMoneyL;
    dataY = dbY3 + (dbH - icon->GetHeight()) / 2;
    icon->SetPosition(dataX, dataY);

    ss.str(std::string());
    ss.clear();
    ss.width(digitsBuy);
    ss.fill(digitsFill);
    ss << g->GetResourcePriceBuy(RES_DIAMONDS);

    mLabelTotBuyDiamonds = new sgui::Label(ss.str().c_str(), fontData, this);
    mLabelTotBuyDiamonds->SetColor(colorData);

    dataX += icon->GetWidth() + marginIconMoneyR;
    dataY = dbY3 + (dbH - mLabelTotBuyDiamonds->GetHeight()) / 2;
    mLabelTotBuyDiamonds->SetPosition(dataX, dataY);

    // TOTAL BUY
    tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_ICON_MONEY);
    icon = new sgui::Image(tex, this);

    dataX = hbX2 + marginIconMoneyL;
    dataY = dbY4 + (dbH - icon->GetHeight()) / 2;
    icon->SetPosition(dataX, dataY);

    ss.str(std::string());
    ss.clear();
    ss.width(digitsTot);
    ss.fill(digitsFill);
    ss << zero;

    mLabelTotBuy = new sgui::Label(ss.str().c_str(), fontData, this);
    mLabelTotBuy->SetColor(colorData);

    dataX += icon->GetWidth() + marginIconMoneyR;
    dataY = dbY4 + (dbH - mLabelTotBuy->GetHeight()) / 2;
    mLabelTotBuy->SetPosition(dataX, dataY);

    // -- LABELS TOTAL SELL --
    ss.str(std::string());
    ss.clear();

    // ENERGY
    tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_ICON_MONEY);
    icon = new sgui::Image(tex, this);

    dataX = hbX3 + marginIconMoneyL;
    dataY = dbY0 + (dbH - icon->GetHeight()) / 2;
    icon->SetPosition(dataX, dataY);

    ss.width(digitsBuy);
    ss.fill(digitsFill);
    ss << g->GetResourcePriceSell(RES_ENERGY);

    mLabelTotBuyEnergy = new sgui::Label(ss.str().c_str(), fontData, this);
    mLabelTotBuyEnergy->SetColor(colorData);

    dataX += icon->GetWidth() + marginIconMoneyR;
    dataY = dbY0 + (dbH - mLabelTotBuyEnergy->GetHeight()) / 2;
    mLabelTotBuyEnergy->SetPosition(dataX, dataY);

    // MATERIAL
    tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_ICON_MONEY);
    icon = new sgui::Image(tex, this);

    dataX = hbX3 + marginIconMoneyL;
    dataY = dbY1 + (dbH - icon->GetHeight()) / 2;
    icon->SetPosition(dataX, dataY);

    ss.str(std::string());
    ss.clear();
    ss.width(digitsBuy);
    ss.fill(digitsFill);
    ss << g->GetResourcePriceSell(RES_MATERIAL1);

    mLabelTotBuyMaterial = new sgui::Label(ss.str().c_str(), fontData, this);
    mLabelTotBuyMaterial->SetColor(colorData);

    dataX += icon->GetWidth() + marginIconMoneyR;
    dataY = dbY1 + (dbH - mLabelTotBuyMaterial->GetHeight()) / 2;
    mLabelTotBuyMaterial->SetPosition(dataX, dataY);

    // BLOBS
    tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_ICON_MONEY);
    icon = new sgui::Image(tex, this);

    dataX = hbX3 + marginIconMoneyL;
    dataY = dbY2 + (dbH - icon->GetHeight()) / 2;
    icon->SetPosition(dataX, dataY);

    ss.str(std::string());
    ss.clear();
    ss.width(digitsBuy);
    ss.fill(digitsFill);
    ss << g->GetResourcePriceSell(RES_BLOBS);

    mLabelTotBuyBlobs = new sgui::Label(ss.str().c_str(), fontData, this);
    mLabelTotBuyBlobs->SetColor(colorData);

    dataX += icon->GetWidth() + marginIconMoneyR;
    dataY = dbY2 + (dbH - mLabelTotBuyBlobs->GetHeight()) / 2;
    mLabelTotBuyBlobs->SetPosition(dataX, dataY);

    // DIAMONDS
    tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_ICON_MONEY);
    icon = new sgui::Image(tex, this);

    dataX = hbX3 + marginIconMoneyL;
    dataY = dbY3 + (dbH - icon->GetHeight()) / 2;
    icon->SetPosition(dataX, dataY);

    ss.str(std::string());
    ss.clear();
    ss.width(digitsBuy);
    ss.fill(digitsFill);
    ss << g->GetResourcePriceSell(RES_DIAMONDS);

    mLabelTotBuyDiamonds = new sgui::Label(ss.str().c_str(), fontData, this);
    mLabelTotBuyDiamonds->SetColor(colorData);

    dataX += icon->GetWidth() + marginIconMoneyR;
    dataY = dbY3 + (dbH - mLabelTotBuyDiamonds->GetHeight()) / 2;
    mLabelTotBuyDiamonds->SetPosition(dataX, dataY);

    // TOTAL SELL
    tex = tm->GetSprite(SpriteFileDialogTrading, ID_DLG_TRADING_ICON_MONEY);
    icon = new sgui::Image(tex, this);

    dataX = hbX3 + marginIconMoneyL;
    dataY = dbY4 + (dbH - icon->GetHeight()) / 2;
    icon->SetPosition(dataX, dataY);

    ss.str(std::string());
    ss.clear();
    ss.width(digitsTot);
    ss.fill(digitsFill);
    ss << zero;

    mLabelTotSell = new sgui::Label(ss.str().c_str(), fontData, this);
    mLabelTotSell->SetColor(colorData);

    dataX += icon->GetWidth() + marginIconMoneyR;
    dataY = dbY4 + (dbH - mLabelTotSell->GetHeight()) / 2;
    mLabelTotSell->SetPosition(dataX, dataY);
}

void DialogTrading::SetFunctionOnClose(const std::function<void()> & f)
{
    mButtonClose->AddOnClickFunction(f);
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

} // namespace game
