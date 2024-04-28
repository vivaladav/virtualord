#pragma once

#include "GameObjects/GameObjectTypes.h"

#include <sgl/sgui/Widget.h>

#include <array>
#include <functional>
#include <vector>

namespace sgl
{
    namespace graphic { class Image; }

    namespace sgui
    {
        class AbstractButton;
        class AbstractButtonsGroup;
        class ButtonsGroup;
        class Label;
        class TextArea;
    }
}

namespace game
{

class ButtonBuild;
class ButtonClose;
class ButtonLeft;
class ButtonRight;
class ButtonSlot;
class ObjectsDataRegistry;
class ObjectVisualAttribute;
class Player;

enum ObjFamily : unsigned int;

class DialogNewElement : public sgl::sgui::Widget
{
public:
    enum ElemType : unsigned int
    {
        ETYPE_UNITS_BARRACKS,
        ETYPE_UNITS_BASE,
        ETYPE_UNITS_HOSPITAL,
        ETYPE_STRUCTURES,

        NUM_ELEMENT_TYPES
    };

    DialogNewElement(ElemType type, Player * player, const ObjectsDataRegistry * dataReg);

    void CheckBuild();

    void SetFunctionOnBuild(const std::function<void()> & f);
    void SetFunctionOnClose(const std::function<void()> & f);

    sgl::sgui::AbstractButton * GetButtonBuild() const;

    int GetSelectedIndex() const;
    GameObjectTypeId GetSelectedType() const;

private:
    void UpdateSlots();

    void ShowStructuresByFamily(ObjFamily fam);
    void ShowData(int ind);

    void CheckBuild(int ind);

    void HandlePositionChanged() override;
    void PositionElements();

private:
    static const int VIS_ATT_ROWS = 7;
    static const int VIS_ATT_COLS = 2;
    static const int NUM_VIS_ATT = VIS_ATT_ROWS * VIS_ATT_COLS;

    static const int NUM_COSTS = 4;

private:
    std::array<ObjectVisualAttribute *, NUM_VIS_ATT> mVisAtt;
    std::array<sgl::sgui::Label *, NUM_COSTS> mLabelsCost;

    std::vector<GameObjectTypeId> mTypes;

    sgl::graphic::Image * mBgTop = nullptr;
    sgl::graphic::Image * mBgMid = nullptr;
    sgl::graphic::Image * mBgBtm = nullptr;

    sgl::sgui::Label * mTitle = nullptr;
    sgl::sgui::ButtonsGroup * mSlots = nullptr;

    sgl::sgui::TextArea * mDescription = nullptr;
    sgl::sgui::Label * mCategory = nullptr;

    ButtonBuild * mBtnBuild = nullptr;
    ButtonClose * mBtnClose = nullptr;
    ButtonLeft * mBtnLeft = nullptr;
    ButtonRight * mBtnRight = nullptr;

    sgl::sgui::AbstractButtonsGroup * mButtonsStructures = nullptr;

    Player * mPlayer = nullptr;

    const ObjectsDataRegistry * mDataReg;

    ElemType mElemType;

    int mFirstElem = 0;
};

} // namespace game
