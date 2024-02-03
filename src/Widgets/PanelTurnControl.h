#pragma once

#include <sgl/sgui/Widget.h>

namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui
    {
        class Image;
        class ImageButton;
        class ProgressBar;
    }
}

namespace game
{

class DigitsDisplay;
class Player;

class PanelTurnControl : public sgl::sgui::Widget
{
public:
    PanelTurnControl(Player * player, sgl::sgui::Widget * parent);

private:
    void HandlePositionChanged() override;

private:
    sgl::graphic::Image * mBg = nullptr;

    sgl::sgui::Image * mIconEnergy = nullptr;
    sgl::sgui::ProgressBar * mEnergyBar = nullptr;
    DigitsDisplay * mDigits = nullptr;
    sgl::sgui::ImageButton * mButtonEndTurn = nullptr;

    Player * mPlayer = nullptr;
};

} // namespace game
