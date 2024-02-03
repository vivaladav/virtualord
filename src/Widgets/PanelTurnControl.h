#pragma once

#include <sgl/sgui/Widget.h>

#include <functional>

namespace sgl
{
    namespace graphic { class Image; }

    namespace sgui
    {
        class Image;
        class ImageButton;
        class Label;
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
    ~PanelTurnControl();

    void SetFunctionEndTurn(const std::function<void()> & f);

    void SetButtonEndTurnEnabled(bool enabled);

    void ShowPanel();
    void ShowText();

private:
    void HandlePositionChanged() override;

private:
    sgl::graphic::Image * mBg = nullptr;

    sgl::sgui::Image * mIconEnergy = nullptr;
    sgl::sgui::ProgressBar * mEnergyBar = nullptr;
    DigitsDisplay * mDigits = nullptr;
    sgl::sgui::ImageButton * mButtonEndTurn = nullptr;

    sgl::sgui::Label * mTextEnemyTurn = nullptr;

    Player * mPlayer = nullptr;
};

} // namespace game
