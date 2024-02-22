#pragma once

#include <sgl/sgui/Widget.h>

#include <array>
#include <functional>

namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui
    {
        class AbstractButton;
        class Image;
        class Label;
    }
}

namespace game
{

class GameObject;
class ObjectsDataRegistry;
class ObjectExtendedVisualRank;
class ObjectExtendedVisualStat;
class ObjectVisualAttribute;

class DialogObject : public sgl::sgui::Widget
{
public:
    DialogObject(const ObjectsDataRegistry * odr);

    void SetFunctionOnClose(const std::function<void()> & f);

    void SetObject(GameObject * obj);

protected:
    void HandlePositionChanged() override;

private:
    void SetPositions();

private:
    static const int VIS_ATT_ROWS = 6;
    static const int VIS_ATT_COLS = 2;
    static const int NUM_VIS_ATT = VIS_ATT_ROWS * VIS_ATT_COLS;

    static const int NUM_COSTS = 4;

private:
    std::array<ObjectVisualAttribute *, NUM_VIS_ATT> mVisAtt;

    const ObjectsDataRegistry * mObjDataReg;

    sgl::graphic::Image * mBg = nullptr;
    sgl::sgui::AbstractButton * mBtnClose = nullptr;

    sgl::sgui::Label * mTitle = nullptr;
    sgl::sgui::Image * mImg = nullptr;

    ObjectExtendedVisualRank * mStatRank = nullptr;
    ObjectExtendedVisualStat * mStatExperience = nullptr;
    ObjectExtendedVisualStat * mStatEnergy = nullptr;
    ObjectExtendedVisualStat * mStatHealth = nullptr;
};

} // namespace game
