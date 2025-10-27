#include "Widgets/PanelObjectActions.h"

#include "GameObjects/Unit.h"
#include "GameObjects/WallGate.h"
#include "Widgets/ObjectActionButton.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Renderer.h>

namespace game
{

PanelObjectActions::PanelObjectActions(sgl::sgui::Widget * parent)
    : sgl::sgui::Widget(parent)
{
    using namespace sgl::core;

    SetResizePolicy(ResizePolicy::DYNAMIC);

    // create all buttons
    mButtons[BTN_BUILD_UNIT_BARRACKS] = new ObjectActionButton(ObjectActionButton::UNITS, "U", KeyboardEvent::KEY_U,
                                                               "Create a new soldier", this);
    mButtons[BTN_BUILD_UNIT_BASE] = new ObjectActionButton(ObjectActionButton::UNITS, "U", KeyboardEvent::KEY_U,
                                                           "Create a new worker", this);
    mButtons[BTN_BUILD_UNIT_HOSPITAL] = new ObjectActionButton(ObjectActionButton::UNITS, "U", KeyboardEvent::KEY_U,
                                                               "Create a new medic", this);
    mButtons[BTN_MISSION_GOALS] = new ObjectActionButton(ObjectActionButton::MISSION_GOALS, "G", KeyboardEvent::KEY_G,
                                                         "Show mission goals", this);
    mButtons[BTN_MOVE] = new ObjectActionButton(ObjectActionButton::MOVE, "M", KeyboardEvent::KEY_M,
                                                "Move your unit", this);
    mButtons[BTN_ATTACK] = new ObjectActionButton(ObjectActionButton::ATTACK, "K", KeyboardEvent::KEY_K,
                                                  "Attack a target", this);
    mButtons[BTN_HEAL_HOSPITAL] = new ObjectActionButton(ObjectActionButton::HEAL, "H", KeyboardEvent::KEY_H,
                                                         "Heal a target", this);
    mButtons[BTN_HEAL_UNIT] = new ObjectActionButton(ObjectActionButton::HEAL, "H", KeyboardEvent::KEY_H,
                                                     "Heal a target", this);
    mButtons[BTN_CONQUER_CELL] = new ObjectActionButton(ObjectActionButton::CONQUER_CELL, "C", KeyboardEvent::KEY_C,
                                                        "Conquer cells", this);
    mButtons[BTN_BUILD_WALL] = new ObjectActionButton(ObjectActionButton::BUILD_WALL, "L", KeyboardEvent::KEY_L,
                                                      "Build a wall", this);
    mButtons[BTN_BUILD_STRUCT] = new ObjectActionButton(ObjectActionButton::BUILD_STRUCT, "B", KeyboardEvent::KEY_B,
                                                        "Build a structure", this);
    mButtons[BTN_UPGRADE] = new ObjectActionButton(ObjectActionButton::UPGRADE, "U", KeyboardEvent::KEY_U,
                                                   "Upgrade", this);
    mButtons[BTN_OPEN_GATE] = new ObjectActionButton(ObjectActionButton::OPEN_GATE, "G", KeyboardEvent::KEY_G,
                                                     "Open the gate", this);
    mButtons[BTN_CLOSE_GATE] = new ObjectActionButton(ObjectActionButton::CLOSE_GATE, "G", KeyboardEvent::KEY_G,
                                                      "Close the gate", this);
    mButtons[BTN_TRADE] = new ObjectActionButton(ObjectActionButton::TRADE, "T", KeyboardEvent::KEY_T,
                                                      "Trade your resources", this);

    mButtons[BTN_CANCEL] = new ObjectActionButton(ObjectActionButton::CANCEL, "X", KeyboardEvent::KEY_X,
                                                  "Cancel current action", this);
}

PanelObjectActions::~PanelObjectActions()
{
}

void PanelObjectActions::ClearObject()
{
}

void PanelObjectActions::SetObject(GameObject * obj)
{
    mObj = obj;

    // reset all buttons
    for(ObjectActionButton * btn : mButtons)
        btn->SetVisible(false);

    // cancel is always visible
    mButtons[BTN_CANCEL]->SetVisible(true);

    // ENABLE BUTTONS
    const GameObjectTypeId objType = mObj->GetObjectType();

    if(objType == GameObject::TYPE_BASE)
    {
        mButtons[BTN_MISSION_GOALS]->SetVisible(true);
        mButtons[BTN_BUILD_UNIT_BASE]->SetVisible(true);
    }
    else if(mObj->GetObjectCategory() == GameObject::CAT_UNIT)
    {
        auto unit = static_cast<Unit *>(mObj);

        mButtons[BTN_MOVE]->SetVisible(true);

        if(unit->CanAttack())
            mButtons[BTN_ATTACK]->SetVisible(true);

        if(unit->CanBuild())
        {
            mButtons[BTN_BUILD_WALL]->SetVisible(true);
            mButtons[BTN_BUILD_STRUCT]->SetVisible(true);
        }

        if(unit->CanConquer())
            mButtons[BTN_CONQUER_CELL]->SetVisible(true);

        if(unit->CanHeal())
            mButtons[BTN_HEAL_UNIT]->SetVisible(true);
    }
    else if(objType == GameObject::TYPE_BARRACKS)
    {
        if(obj->IsLinked())
            mButtons[BTN_BUILD_UNIT_BARRACKS]->SetVisible(true);
        else
            mButtons[BTN_CANCEL]->SetVisible(false);
    }
    else if(objType == GameObject::TYPE_HOSPITAL)
    {
        if(obj->IsLinked())
        {
            mButtons[BTN_BUILD_UNIT_HOSPITAL]->SetVisible(true);
            mButtons[BTN_HEAL_HOSPITAL]->SetVisible(true);
        }
        else
            mButtons[BTN_CANCEL]->SetVisible(false);
    }
    else if(objType == GameObject::TYPE_TRADING_POST)
    {
        if(obj->IsLinked())
            mButtons[BTN_TRADE]->SetVisible(true);
        else
            mButtons[BTN_CANCEL]->SetVisible(false);
    }
    else if(objType == GameObject::TYPE_WALL_GATE)
    {
        auto gate = static_cast<WallGate *>(mObj);

        if(gate->IsOpen())
            mButtons[BTN_CLOSE_GATE]->SetVisible(true);
        else
            mButtons[BTN_OPEN_GATE]->SetVisible(true);
    }

    // object not supported -> hide all buttons
    else
        mButtons[BTN_CANCEL]->SetVisible(false);

    // POSITION BUTTONS
    const int marginH = 15;
    int x = 0;

    for(ObjectActionButton * btn : mButtons)
    {
        if(btn->IsVisible())
        {
            btn->SetX(x);

            x += btn->GetWidth() + marginH;
        }
    }

    // update position
    const int rendW = sgl::graphic::Renderer::Instance()->GetWidth();
    const int rendH = sgl::graphic::Renderer::Instance()->GetHeight();
    const int marginB = 150;

    const int panelX = (rendW - GetWidth()) * 0.5f;
    const int panelY = rendH - GetHeight() - marginB;

    SetPosition(panelX, panelY);
}

unsigned int PanelObjectActions::AddButtonFunction(Button btnId, const std::function<void()> & f)
{
    if(btnId < NUM_BUTTONS)
       return mButtons[btnId]->AddOnClickFunction(f);
    else
        return 0;
}

void PanelObjectActions::RemoveButtonFunction(Button btnId, unsigned int funId)
{
    if(btnId < NUM_BUTTONS)
       mButtons[btnId]->RemoveClickFunction(funId);
}

void PanelObjectActions::SetActionsEnabled(bool val)
{
    for(unsigned int i = 0; i < static_cast<unsigned int>(BTN_CANCEL); ++i)
        mButtons[i]->SetEnabled(val);
}

const sgl::sgui::AbstractButton * PanelObjectActions::GetButton(Button btnId)
{
    if(btnId < NUM_BUTTONS)
        return mButtons[btnId];
    else
        return nullptr;
}

} // namespace game
