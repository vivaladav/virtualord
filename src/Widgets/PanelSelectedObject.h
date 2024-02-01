#include <sgl/sgui/Widget.h>

#include <functional>

namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui
    {
        class Image;
        class ImageButton;
        class TextArea;
    }
}

namespace game
{

class GameObject;
class ObjectsDataRegistry;

class PanelSelectedObject : public sgl::sgui::Widget
{

public:
    PanelSelectedObject(const ObjectsDataRegistry * odr, sgl::sgui::Widget * parent);

    void AddFunctionOnClose(const std::function<void()> & f);

    void SetObject(GameObject * obj);

private:
    void HandlePositionChanged() override;
    void PositionElements();

private:
    const ObjectsDataRegistry * mObjDataReg;

    sgl::graphic::Image * mBg = nullptr;

    sgl::sgui::ImageButton * mButtonClose = nullptr;

    sgl::sgui::Image * mImg = nullptr;
    sgl::sgui::TextArea * mTitle = nullptr;
    sgl::sgui::Image * mBarLvl = nullptr;

    GameObject * mObj = nullptr;
};

} // namespace game
