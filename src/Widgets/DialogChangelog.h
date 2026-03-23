#pragma once

#include "Widgets/GameButton.h"

#include <functional>

namespace sgl
{
    namespace graphic
    {
        class Image;
        class Text;
    }

    namespace sgui
    {
        class Label;
        class ScrollArea;
        class TextArea;
    }
}

namespace game
{

// ===== BUTTON CHANGELOG =====
class ButtonChangelog : public GameButton
{
public:
    ButtonChangelog();
};

// ===== DIALOG CHANGELOG =====
class DialogChangelog : public sgl::sgui::Widget
{
public:
    DialogChangelog();

    void SetContent(sgl::sgui::Widget * cont);

    unsigned int AddOnCloseClickFunction(const std::function<void()> & f);

private:
    void HandlePositionChanged() override;

    void PositionElements();

private:
    sgl::graphic::Image * mBg = nullptr;
    sgl::graphic::Text * mTitle = nullptr;
    sgl::sgui::ScrollArea * mContArea = nullptr;
    sgl::sgui::AbstractButton * mButtonClose = nullptr;
};

} // namespace game
