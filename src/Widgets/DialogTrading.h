#pragma once

#include <sgl/sgui/Widget.h>

#include <array>
#include <functional>

namespace sgl
{
    namespace graphic
    {
        class Font;
        class Image;
    }
    namespace sgui
    {
        class AbstractButton;
        class Label;
    }
}

namespace game
{

class Game;
class Player;
class Screen;

class DialogTrading : public sgl::sgui::Widget
{
public:
    DialogTrading(Game * g, Player * p);
    ~DialogTrading();

    void SetFunctionOnClose(const std::function<void()> & f);

private:
    enum TradedResources : unsigned int
    {
        TR_ENERGY,
        TR_MATERIAL,
        TR_DIAMONDS,
        TR_BLOBS,
        TR_RESEARCH,

        NUM_TRADED_RES,

        TR_NULL
    };

    void AddResBlock(int x0, int y0, TradedResources res, sgl::graphic::Font * font);
    void AddStockBlock(int x0, int y0, int bW, TradedResources res, sgl::graphic::Font * font);
    void AddBuyBlock(int x0, int y0, int bW, TradedResources res, sgl::graphic::Font * font);
    void AddSellBlock(int x0, int y0, int bW, TradedResources res, sgl::graphic::Font * font);

    void HandlePositionChanged() override;

    void SetPositions();

    int GetCurrentSpend() const;
    void UpdateLabelTotalSpend();

    int GetCurrentGain() const;
    void UpdateLabelTotalGain();

    void IncBuyQuantity(TradedResources res, sgl::sgui::Label * label);
    void DecBuyQuantity(TradedResources res, sgl::sgui::Label * label);
    void IncSellQuantity(TradedResources res, sgl::sgui::Label * label);
    void DecSellQuantity(TradedResources res, sgl::sgui::Label * label);

    void UpdateStockLabel(unsigned int statId);

    void UpdateButtons();

    void Buy();
    void Sell();

private:
    std::array<int, NUM_TRADED_RES> mBuy;
    std::array<int, NUM_TRADED_RES> mSell;

    std::array<sgl::sgui::Label *, NUM_TRADED_RES> mLabelStock;
    std::array<sgl::sgui::Label *, NUM_TRADED_RES> mLabelBuy;
    std::array<sgl::sgui::Label *, NUM_TRADED_RES> mLabelSell;

    sgl::graphic::Image * mBgL = nullptr;
    sgl::graphic::Image * mBgC = nullptr;
    sgl::graphic::Image * mBgR = nullptr;

    sgl::sgui::AbstractButton * mButtonClose = nullptr;

    std::array<sgl::sgui::AbstractButton *, NUM_TRADED_RES> mButtonsBuyMinus;
    std::array<sgl::sgui::AbstractButton *, NUM_TRADED_RES> mButtonsBuyPlus;
    std::array<sgl::sgui::AbstractButton *, NUM_TRADED_RES> mButtonsSellMinus;
    std::array<sgl::sgui::AbstractButton *, NUM_TRADED_RES> mButtonsSellPlus;

    sgl::sgui::AbstractButton * mButtonBuy = nullptr;
    sgl::sgui::AbstractButton * mButtonSell = nullptr;

    sgl::sgui::Label * mLabelTotBuy = nullptr;
    sgl::sgui::Label * mLabelTotSell = nullptr;

    Game * mGame = nullptr;
    Player * mPlayer = nullptr;

    std::array<unsigned int, NUM_TRADED_RES> mCallbackValIds;
};

} // namespace game
