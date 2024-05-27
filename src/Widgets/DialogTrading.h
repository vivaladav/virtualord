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
    DialogTrading(Player * p);

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

    Player * mPlayer = nullptr;
};

} // namespace game
