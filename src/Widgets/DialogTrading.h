#pragma once

#include <sgl/sgui/Widget.h>

#include <functional>

namespace sgl
{
    namespace graphic { class Image; }
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

    void SetFunctionOnClose(const std::function<void()> & f);

private:
    void HandlePositionChanged() override;

    void SetPositions();

    int GetCurrentSpend() const;
    void UpdateLabelTotalSpend();

    int GetCurrentGain() const;
    void UpdateLabelTotalGain();

private:
    sgl::graphic::Image * mBg = nullptr;
    sgl::sgui::AbstractButton * mButtonClose = nullptr;

    sgl::sgui::Label * mLabelStockEnergy = nullptr;
    sgl::sgui::Label * mLabelStockMaterial = nullptr;
    sgl::sgui::Label * mLabelStockBlobs = nullptr;
    sgl::sgui::Label * mLabelStockDiamonds = nullptr;

    sgl::sgui::Label * mLabelTotBuy = nullptr;
    sgl::sgui::Label * mLabelTotSell = nullptr;

    int mBuyEnergy = 0;
    int mBuyMaterial = 0;
    int mBuyBlobs = 0;
    int mBuyDiamonds = 0;

    int mSellEnergy = 0;
    int mSellMaterial = 0;
    int mSellBlobs = 0;
    int mSellDiamonds = 0;

    Game * mGame = nullptr;
    Player * mPlayer = nullptr;
};

} // namespace game
