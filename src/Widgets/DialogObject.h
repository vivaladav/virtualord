#pragma once

#include <sgl/sgui/Widget.h>

#include <functional>

namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui
    {
        class AbstractButton;
        class Label;
    }
}

namespace game
{

class GameObject;

class DialogObject : public sgl::sgui::Widget
{
public:
    DialogObject();

    void SetFunctionOnClose(const std::function<void()> & f);

    void SetObject(GameObject * obj);

protected:
    void HandlePositionChanged() override;

private:
    void SetPositions();

private:
    sgl::graphic::Image * mBg = nullptr;
    sgl::sgui::AbstractButton * mBtnClose = nullptr;

    sgl::sgui::Label * mTitle = nullptr;
};

} // namespace game
