#pragma once

#include <sgl/sgui/Widget.h>

#include <array>
#include <functional>

namespace sgl
{
    namespace graphic { class Image; }
}

namespace game
{

class GameObject;
class ObjectActionButton;

class PanelObjectActions : public sgl::sgui::Widget
{
public:
    enum Button
    {
        BTN_BUILD_UNIT_BARRACKS,
        BTN_BUILD_UNIT_BASE,
        BTN_BUILD_UNIT_HOSPITAL,
        BTN_MOVE,
        BTN_ATTACK,
        BTN_HEAL,
        BTN_CONQUER_CELL,
        BTN_BUILD_WALL,
        BTN_BUILD_STRUCT,
        BTN_UPGRADE,
        BTN_OPEN_GATE,
        BTN_CLOSE_GATE,
        BTN_CANCEL,

        NUM_BUTTONS
    };

public:
    PanelObjectActions(sgl::sgui::Widget * parent);
    ~PanelObjectActions();

    void ClearObject();
    void SetObject(GameObject * obj);

    void SetButtonFunction(Button btnId, const std::function<void()> & f);

    void SetActionsEnabled(bool val);

private:
    std::array<ObjectActionButton *, NUM_BUTTONS> mButtons;

    GameObject * mObj = nullptr;
};

} // namespace game
