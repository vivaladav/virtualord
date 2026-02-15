#pragma once

#include <sgl/sgui/PushButton.h>

namespace sgl
{
    namespace graphic
    {
        class Image;
        class Text;
    }
}

namespace game
{

class GameSimpleTooltip;

class ButtonObjectActionOption : public sgl::sgui::PushButton
{
public:
    ButtonObjectActionOption(const char * text, const char * shortcut,
                             int shortcutKey, sgl::sgui::Widget * parent);

    void SetText(const char * text);

    void CreateTooltip(const char * text, int timeShowing);
    void SetTooltipText(const char * text);

private:
    void HandleButtonDown() override;
    void HandleMouseOver() override;

    void HandlePositionChanged() override;
    void SetPositions();

    void OnStateChanged(sgl::sgui::AbstractButton::VisualState state) override;
    void SetGraphics();

private:
    sgl::graphic::Image * mBody = nullptr;
    sgl::graphic::Text * mLabel = nullptr;
    sgl::graphic::Text * mShortcut = nullptr;

    GameSimpleTooltip * mTooltip = nullptr;
};

} // namespace game
