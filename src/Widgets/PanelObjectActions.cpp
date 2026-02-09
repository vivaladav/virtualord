#include "Widgets/PanelObjectActions.h"

#include "GameObjects/Unit.h"
#include "GameObjects/WallGate.h"
#include "Widgets/ObjectActionButton.h"

#include <sgl/core/event/KeyboardEvent.h>
#include <sgl/graphic/Renderer.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

PanelObjectActions::PanelObjectActions(sgl::sgui::Widget * parent)
    : sgl::sgui::Widget(parent)
{
    using namespace sgl::core;

    auto sm = sgl::utilities::StringManager::Instance();
    sm->AddListener(this);

    SetResizePolicy(ResizePolicy::DYNAMIC);

    // create all buttons
    mButtons[BTN_BUILD_UNIT_BARRACKS] =
        new ObjectActionButton(ObjectActionButton::UNITS, "U", KeyboardEvent::KEY_U,
                               sm->GetCString("TT_NEW_UNIT"), this);
    mButtons[BTN_BUILD_UNIT_BASE] =
        new ObjectActionButton(ObjectActionButton::UNITS, "U", KeyboardEvent::KEY_U,
                               sm->GetCString("TT_NEW_UNIT"), this);
    mButtons[BTN_BUILD_UNIT_HOSPITAL] =
        new ObjectActionButton(ObjectActionButton::UNITS, "U", KeyboardEvent::KEY_U,
                               sm->GetCString("TT_NEW_UNIT"), this);
    mButtons[BTN_MISSION_GOALS] =
        new ObjectActionButton(ObjectActionButton::MISSION_GOALS, "G", KeyboardEvent::KEY_G,
                               sm->GetCString("TT_SHOW_GOALS"), this);
    mButtons[BTN_MOVE] =
        new ObjectActionButton(ObjectActionButton::MOVE, "M", KeyboardEvent::KEY_M,
                               sm->GetCString("TT_MOVE_UNIT"), this);
    mButtons[BTN_ATTACK] =
        new ObjectActionButton(ObjectActionButton::ATTACK, "K", KeyboardEvent::KEY_K,
                               sm->GetCString("TT_ATTACK_TARGET"), this);
    mButtons[BTN_HEAL_HOSPITAL] =
        new ObjectActionButton(ObjectActionButton::HEAL, "H", KeyboardEvent::KEY_H,
                               sm->GetCString("TT_HEAL_TARGET"), this);
    mButtons[BTN_HEAL_UNIT] =
        new ObjectActionButton(ObjectActionButton::HEAL, "H", KeyboardEvent::KEY_H,
                               sm->GetCString("TT_HEAL_TARGET"), this);
    mButtons[BTN_CONQUER_CELL] =
        new ObjectActionButton(ObjectActionButton::CONQUER_CELL, "C", KeyboardEvent::KEY_C,
                               sm->GetCString("TT_CONQ_CELLS"), this);
    mButtons[BTN_BUILD_WALL] =
        new ObjectActionButton(ObjectActionButton::BUILD_WALL, "L", KeyboardEvent::KEY_L,
                               sm->GetCString("TT_BUIL_WALL"), this);
    mButtons[BTN_BUILD_STRUCT] =
        new ObjectActionButton(ObjectActionButton::BUILD_STRUCT, "B", KeyboardEvent::KEY_B,
                               sm->GetCString("TT_BUILD_STRUCT"), this);
    mButtons[BTN_UPGRADE] =
        new ObjectActionButton(ObjectActionButton::UPGRADE, "P", KeyboardEvent::KEY_P,
                               sm->GetCString("TT_UPGRADE"), this);
    mButtons[BTN_OPEN_GATE] =
        new ObjectActionButton(ObjectActionButton::OPEN_GATE, "G", KeyboardEvent::KEY_G,
                               sm->GetCString("TT_OPEN_GATE"), this);
    mButtons[BTN_CLOSE_GATE] =
        new ObjectActionButton(ObjectActionButton::CLOSE_GATE, "G", KeyboardEvent::KEY_G,
                               sm->GetCString("TT_CLOSE_GATE"), this);
    mButtons[BTN_TRADE] =
        new ObjectActionButton(ObjectActionButton::TRADE, "T", KeyboardEvent::KEY_T,
                               sm->GetCString("TT_TRADE_RES"), this);
    mButtons[BTN_SPAWN] =
        new ObjectActionButton(ObjectActionButton::SPAWN, "N", KeyboardEvent::KEY_N,
                               sm->GetCString("TT_SPAWN_MU"), this);
    mButtons[BTN_SET_TARGET] =
        new ObjectActionButton(ObjectActionButton::SET_TARGET, "T", KeyboardEvent::KEY_T,
                               sm->GetCString("TT_SET_DEST"), this);
    mButtons[BTN_RESEARCH] =
        new ObjectActionButton(ObjectActionButton::RESEARCH, "R", KeyboardEvent::KEY_R,
                               sm->GetCString("TT_RESEARCH"), this);

    // keep these 2 last
    mButtons[BTN_SELF_DESTROY] =
        new ObjectActionButton(ObjectActionButton::SELF_DESTROY, "E", KeyboardEvent::KEY_E,
                               sm->GetCString("TT_SELF_DESTR"), this);
    mButtons[BTN_CANCEL] =
        new ObjectActionButton(ObjectActionButton::CANCEL, "X", KeyboardEvent::KEY_X,
                               sm->GetCString("TT_CANCEL"), this);
}

void PanelObjectActions::SetObject(GameObject * obj)
{
    ClearObserving();

    mObj = obj;

    mFuncValuesChangedId = mObj->AddFunctionOnValueChanged([this]
    {
        OnUpdateStats();
    });

    // reset all buttons
    for(ObjectActionButton * btn : mButtons)
        btn->SetVisible(false);

    // cancel is always visible
    mButtons[BTN_CANCEL]->SetVisible(true);

    // ENABLE BUTTONS
    const GameObjectTypeId objType = mObj->GetObjectType();

    // self destruction available for all, but base
    if(objType != ObjectData::TYPE_BASE)
        mButtons[BTN_SELF_DESTROY]->SetVisible(true);

    if(objType == ObjectData::TYPE_BASE)
    {
        mButtons[BTN_MISSION_GOALS]->SetVisible(true);
        mButtons[BTN_BUILD_UNIT_BASE]->SetVisible(true);
    }
    else if(mObj->GetObjectCategory() == ObjectData::CAT_UNIT)
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

        if(unit->CanSpawn())
            mButtons[BTN_SPAWN]->SetVisible(true);
    }
    else if(mObj->GetObjectCategory() == ObjectData::CAT_MINI_UNIT)
    {
        mButtons[BTN_SET_TARGET]->SetVisible(true);
    }
    else if(objType == ObjectData::TYPE_BARRACKS)
    {
        if(obj->IsLinked())
            mButtons[BTN_BUILD_UNIT_BARRACKS]->SetVisible(true);
        else
            mButtons[BTN_CANCEL]->SetVisible(false);
    }
    else if(objType == ObjectData::TYPE_RESEARCH_CENTER)
    {
        if(obj->IsLinked())
            mButtons[BTN_RESEARCH]->SetVisible(true);
        else
            mButtons[BTN_CANCEL]->SetVisible(false);
    }
    else if(objType == ObjectData::TYPE_HOSPITAL)
    {
        if(obj->IsLinked())
        {
            mButtons[BTN_BUILD_UNIT_HOSPITAL]->SetVisible(true);
            mButtons[BTN_HEAL_HOSPITAL]->SetVisible(true);
        }
        else
            mButtons[BTN_CANCEL]->SetVisible(false);
    }
    else if(objType == ObjectData::TYPE_SPAWN_TOWER)
    {
        if(obj->IsLinked())
            mButtons[BTN_SPAWN]->SetVisible(true);
        else
            mButtons[BTN_CANCEL]->SetVisible(false);
    }
    else if(objType == ObjectData::TYPE_TRADING_POST)
    {
        if(obj->IsLinked())
            mButtons[BTN_TRADE]->SetVisible(true);
        else
            mButtons[BTN_CANCEL]->SetVisible(false);
    }
    else if(objType == ObjectData::TYPE_WALL_GATE)
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

    // button upgrade
    UpdateButtonUpgrade();

    // POSITION BUTTONS
    UpdatePositions();
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

void PanelObjectActions::OnStringsChanged()
{
    auto sm = sgl::utilities::StringManager::Instance();

    mButtons[BTN_BUILD_UNIT_BARRACKS]->SetTooltipText(sm->GetCString("TT_NEW_UNIT"));
    mButtons[BTN_BUILD_UNIT_BASE]->SetTooltipText(sm->GetCString("TT_NEW_UNIT"));
    mButtons[BTN_BUILD_UNIT_HOSPITAL]->SetTooltipText(sm->GetCString("TT_NEW_UNIT"));
    mButtons[BTN_MISSION_GOALS]->SetTooltipText(sm->GetCString("TT_SHOW_GOALS"));
    mButtons[BTN_MOVE]->SetTooltipText(sm->GetCString("TT_MOVE_UNIT"));
    mButtons[BTN_ATTACK]->SetTooltipText(sm->GetCString("TT_ATTACK_TARGET"));
    mButtons[BTN_HEAL_HOSPITAL]->SetTooltipText(sm->GetCString("TT_HEAL_TARGET"));
    mButtons[BTN_HEAL_UNIT]->SetTooltipText(sm->GetCString("TT_HEAL_TARGET"));
    mButtons[BTN_CONQUER_CELL]->SetTooltipText(sm->GetCString("TT_CONQ_CELLS"));
    mButtons[BTN_BUILD_WALL]->SetTooltipText(sm->GetCString("TT_BUIL_WALL"));
    mButtons[BTN_BUILD_STRUCT]->SetTooltipText(sm->GetCString("TT_BUILD_STRUCT"));
    mButtons[BTN_UPGRADE]->SetTooltipText(sm->GetCString("TT_UPGRADE"));
    mButtons[BTN_OPEN_GATE]->SetTooltipText(sm->GetCString("TT_OPEN_GATE"));
    mButtons[BTN_CLOSE_GATE]->SetTooltipText(sm->GetCString("TT_CLOSE_GATE"));
    mButtons[BTN_TRADE]->SetTooltipText(sm->GetCString("TT_TRADE_RES"));
    mButtons[BTN_SPAWN]->SetTooltipText(sm->GetCString("TT_SPAWN_MU"));
    mButtons[BTN_SET_TARGET]->SetTooltipText(sm->GetCString("TT_SET_DEST"));
    mButtons[BTN_SET_TARGET]->SetTooltipText(sm->GetCString("TT_RESEARCH"));
    mButtons[BTN_SELF_DESTROY]->SetTooltipText(sm->GetCString("TT_SELF_DESTR"));
    mButtons[BTN_CANCEL]->SetTooltipText(sm->GetCString("TT_CANCEL"));
}

void PanelObjectActions::OnUpdateStats()
{
    UpdateButtonUpgrade();

    UpdatePositions();
}

void PanelObjectActions::UpdateButtonUpgrade()
{
    const int exp = mObj->GetExperience();
    const int maxExp = mObj->GetExperienceToNextLevel();
    const bool showUpgrade = exp >= maxExp &&
                             (mObj->GetExperienceLevel() + 1) < GameObject::MAX_LEVEL;

    mButtons[BTN_UPGRADE]->SetVisible(showUpgrade);
}

void PanelObjectActions::ClearObserving()
{
    if(mObj == nullptr)
        return ;

    mObj->RemoveFunctionOnValueChanged(mFuncValuesChangedId);

    mFuncValuesChangedId = 0;
}

void PanelObjectActions::UpdatePositions()
{
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

} // namespace game
