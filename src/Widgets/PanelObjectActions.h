#pragma once

#include <sgl/sgui/Widget.h>
#include <sgl/utilities/StringsChangeListener.h>

#include <array>
#include <functional>

namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui { class AbstractButton; }
}

namespace game
{

class GameObject;
class ObjectActionButton;

class PanelObjectActions : public sgl::sgui::Widget,
                           public sgl::utilities::StringsChangeListener
{
public:
    enum Button : unsigned int
    {
        BTN_BUILD_UNIT_BARRACKS,
        BTN_BUILD_UNIT_BASE,
        BTN_BUILD_UNIT_HOSPITAL,
        BTN_MISSION_GOALS,
        BTN_MOVE,
        BTN_ATTACK,
        BTN_HEAL_HOSPITAL,
        BTN_HEAL_UNIT,
        BTN_CONQUER_CELL,
        BTN_BUILD_WALL,
        BTN_BUILD_STRUCT,
        BTN_UPGRADE,
        BTN_OPEN_GATE,
        BTN_CLOSE_GATE,
        BTN_TRADE,
        BTN_SPAWN,
        BTN_SET_TARGET,
        BTN_RESEARCH,

        // keep these 2 last
        BTN_SELF_DESTROY,
        BTN_CANCEL,

        NUM_BUTTONS
    };

public:
    PanelObjectActions(sgl::sgui::Widget * parent);

    void ClearObject();
    void SetObject(GameObject * obj);
    bool HasObjectSet() const;

    unsigned int AddButtonFunction(Button btnId, const std::function<void()> & f);
    void RemoveButtonFunction(Button btnId, unsigned int funId);

    void SetActionsEnabled(bool val);

    const sgl::sgui::AbstractButton * GetButton(Button btnId);

private:
    void OnStringsChanged() override;

    void OnUpdateStats();
    void UpdateButtonUpgrade();

    void ClearObserving();

    void UpdatePositions();

private:
    std::array<ObjectActionButton *, NUM_BUTTONS> mButtons;

    GameObject * mObj = nullptr;

    unsigned int mFuncValuesChangedId = 0;
};

inline void PanelObjectActions::ClearObject()
{
    ClearObserving();
    mObj = nullptr;
}

inline bool PanelObjectActions::HasObjectSet() const { return mObj != nullptr; }

} // namespace game
