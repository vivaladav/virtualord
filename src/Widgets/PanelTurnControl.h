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

class GameSimpleTooltip;
class Player;

class PanelTurnControl : public sgl::sgui::Widget
{
public:
    PanelTurnControl(Player * player, sgl::sgui::Widget * parent);

    void AddFunctionGoToBase(const std::function<void()> & f);
    void AddFunctionNextUnit(const std::function<void()> & f);
    void AddFunctionEndTurn(const std::function<void()> & f);

    void SetButtonsEnabled(bool enabled);

    void HideText();
    void ShowText(const char * text);

    sgl::sgui::AbstractButton * GetButtonBackToBase() const;
    sgl::sgui::AbstractButton * GetButtonEndTurn() const;

private:
    void HandlePositionChanged() override;

    void PositionText();

private:
    sgl::graphic::Image * mBg = nullptr;

    sgl::sgui::AbstractButton * mButtonBase = nullptr;
    sgl::sgui::AbstractButton * mButtonNextUnit = nullptr;
    sgl::sgui::AbstractButton * mButtonEndTurn = nullptr;

    sgl::sgui::Label * mText = nullptr;

    Player * mPlayer = nullptr;
};

inline sgl::sgui::AbstractButton * PanelTurnControl::GetButtonBackToBase() const { return mButtonBase; }
inline sgl::sgui::AbstractButton * PanelTurnControl::GetButtonEndTurn() const { return mButtonEndTurn; }

} // namespace game
