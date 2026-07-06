#pragma once

#include <sgl/sgui/Widget.h>

#include <functional>

namespace sgl
{
    namespace graphic { class Image; }

    namespace sgui
    {
        class AbstractButton;
        class Image;
        class Label;
        class ProgressBar;
    }
}

namespace game
{

class DigitsDisplay;
class GameSimpleTooltip;
class Player;

class PanelTurnControl : public sgl::sgui::Widget
{
public:
    PanelTurnControl(Player * player, sgl::sgui::Widget * parent);
    ~PanelTurnControl();

    void AddFunctionGoToBase(const std::function<void()> & f);
    void AddFunctionEndTurn(const std::function<void()> & f);

    void SetButtonEndTurnEnabled(bool enabled);

    void ShowPanel();
    void ShowText(const char * text);

    sgl::sgui::AbstractButton * GetButtonBackToBase() const;
    sgl::sgui::AbstractButton * GetButtonEndTurn() const;

private:
    void HandlePositionChanged() override;

    void PositionText();

private:
    sgl::graphic::Image * mBg = nullptr;

    sgl::sgui::Image * mIconEnergy = nullptr;
    sgl::sgui::ProgressBar * mEnergyBar = nullptr;
    DigitsDisplay * mDigits = nullptr;
    sgl::sgui::AbstractButton * mButtonBase = nullptr;
    sgl::sgui::AbstractButton * mButtonEndTurn = nullptr;

    GameSimpleTooltip * mTooltipEnergy = nullptr;

    sgl::sgui::Label * mText = nullptr;

    Player * mPlayer = nullptr;
};

inline sgl::sgui::AbstractButton * PanelTurnControl::GetButtonBackToBase() const { return mButtonBase; }
inline sgl::sgui::AbstractButton * PanelTurnControl::GetButtonEndTurn() const { return mButtonEndTurn; }

} // namespace game
