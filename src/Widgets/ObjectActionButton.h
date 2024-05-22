#pragma once

#include <sgl/sgui/AbstractButton.h>

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

class ObjectActionButton : public sgl::sgui::AbstractButton
{
public:
    enum ActionIcon : unsigned int
    {
        UNITS,
        MOVE,
        ATTACK,
        CONQUER_CELL,
        BUILD_WALL,
        BUILD_STRUCT,
        UPGRADE,
        OPEN_GATE,
        CLOSE_GATE,
        CANCEL,
        HEAL,
        TRADE,

        NUM_ACTION_ICONS
    };

public:
    ObjectActionButton(ActionIcon icon, const char * shortcut, int shortcutKey,
                       const char * tooltip, sgl::sgui::Widget * parent);

private:
    void HandleMouseOver() override;
    void HandleButtonDown() override;

    void OnStateChanged(sgl::sgui::AbstractButton::VisualState state) override;

    void HandlePositionChanged() override;

private:
    sgl::graphic::Image * mBody = nullptr;
    sgl::graphic::Image * mIcon = nullptr;
    sgl::graphic::Text * mShortcut = nullptr;

    ActionIcon mIconId;
};

} // namespace game
