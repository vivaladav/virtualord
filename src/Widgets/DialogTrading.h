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

enum ResourceType : unsigned int;

class DialogTrading : public sgl::sgui::Widget
{
public:
    DialogTrading(Game * g, Player * p);

    void SetFunctionOnClose(const std::function<void()> & f);

private:
    void AddResBlock(int x0, int y0, ResourceType res, sgl::graphic::Font * font);
    void AddStockBlock(int x0, int y0, int bW, ResourceType res, sgl::graphic::Font * font);
    void AddBuyBlock(int x0, int y0, int bW, ResourceType res, sgl::graphic::Font * font);
    void AddSellBlock(int x0, int y0, int bW, ResourceType res, sgl::graphic::Font * font);

    void HandlePositionChanged() override;

    void SetPositions();

    int GetCurrentSpend() const;
    void UpdateLabelTotalSpend();

    int GetCurrentGain() const;
    void UpdateLabelTotalGain();

    void IncBuyQuantity(ResourceType res, sgl::sgui::Label * label);
    void DecBuyQuantity(ResourceType res, sgl::sgui::Label * label);
    void IncSellQuantity(ResourceType res, sgl::sgui::Label * label);
    void DecSellQuantity(ResourceType res, sgl::sgui::Label * label);

    void Buy();
    void Sell();

private:
    static const int TRADED_RES = 4;

    std::array<int, TRADED_RES> mBuy;
    std::array<int, TRADED_RES> mSell;

    std::array<sgl::sgui::Label *, TRADED_RES> mLabelStock;
    std::array<sgl::sgui::Label *, TRADED_RES> mLabelBuy;
    std::array<sgl::sgui::Label *, TRADED_RES> mLabelSell;

    sgl::graphic::Image * mBg = nullptr;
    sgl::sgui::AbstractButton * mButtonClose = nullptr;

    sgl::sgui::Label * mLabelTotBuy = nullptr;
    sgl::sgui::Label * mLabelTotSell = nullptr;


    Game * mGame = nullptr;
    Player * mPlayer = nullptr;
};

} // namespace game
