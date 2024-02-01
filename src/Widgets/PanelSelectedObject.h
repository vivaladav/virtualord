#include <sgl/sgui/Widget.h>

#include <functional>

namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui { class ImageButton; }
}

namespace game
{

class GameObject;

class PanelSelectedObject : public sgl::sgui::Widget
{

public:
    PanelSelectedObject(sgl::sgui::Widget * parent);

    void AddFunctionOnClose(const std::function<void()> & f);

    void SetObject(GameObject * obj);

private:
    void HandlePositionChanged() override;
    void PositionElements();

private:
    sgl::graphic::Image * mBg = nullptr;

    sgl::sgui::ImageButton * mButtonClose = nullptr;

    GameObject * mObj = nullptr;
};

} // namespace game
