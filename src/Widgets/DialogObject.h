#pragma once

#include <sgl/sgui/Widget.h>

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
class ObjectExtendedVisualStat;

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
    const ObjectsDataRegistry * mObjDataReg;

    sgl::graphic::Image * mBg = nullptr;
    sgl::sgui::AbstractButton * mBtnClose = nullptr;

    sgl::sgui::Label * mTitle = nullptr;
    sgl::sgui::Image * mImg = nullptr;

    ObjectExtendedVisualStat * mStatExperience = nullptr;
    ObjectExtendedVisualStat * mStatEnergy = nullptr;
    ObjectExtendedVisualStat * mStatHealth = nullptr;
};

} // namespace game
