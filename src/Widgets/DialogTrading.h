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

protected:
    void HandlePositionChanged() override;

private:
    void SetPositions();

private:
    sgl::graphic::Image * mBg = nullptr;
    sgl::sgui::AbstractButton * mButtonClose = nullptr;

    sgl::sgui::Label * mLabelStockEnergy = nullptr;
    sgl::sgui::Label * mLabelStockMaterial = nullptr;
    sgl::sgui::Label * mLabelStockBlobs = nullptr;
    sgl::sgui::Label * mLabelStockDiamonds = nullptr;

    sgl::sgui::Label * mLabelTotBuyEnergy = nullptr;
    sgl::sgui::Label * mLabelTotBuyMaterial = nullptr;
    sgl::sgui::Label * mLabelTotBuyBlobs = nullptr;
    sgl::sgui::Label * mLabelTotBuyDiamonds = nullptr;
    sgl::sgui::Label * mLabelTotBuy = nullptr;

    sgl::sgui::Label * mLabelTotSellEnergy = nullptr;
    sgl::sgui::Label * mLabelTotSellMaterial = nullptr;
    sgl::sgui::Label * mLabelTotSellBlobs = nullptr;
    sgl::sgui::Label * mLabelTotSellkDiamonds = nullptr;
    sgl::sgui::Label * mLabelTotSell = nullptr;

    Game * mGame = nullptr;
    Player * mPlayer = nullptr;
};

} // namespace game
