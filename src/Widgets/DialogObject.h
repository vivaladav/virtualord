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

    sgl::graphic::Image * mBgL = nullptr;
    sgl::graphic::Image * mBgC = nullptr;
    sgl::graphic::Image * mBgR = nullptr;

    sgl::sgui::AbstractButton * mBtnClose = nullptr;

    sgl::sgui::Widget * mPanelImg = nullptr;
    sgl::sgui::Widget * mPanelStats = nullptr;

    sgl::sgui::Label * mTitle = nullptr;
    sgl::sgui::Image * mImg = nullptr;

    sgl::sgui::Widget * mStatRank = nullptr;
    sgl::sgui::Widget * mStatExperience = nullptr;
    sgl::sgui::Widget * mStatEnergy = nullptr;
    sgl::sgui::Widget * mStatHealth = nullptr;
};

} // namespace game
