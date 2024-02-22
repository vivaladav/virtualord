#include <sgl/sgui/Widget.h>

namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui
    {
        class Image;
        class Label;
    }
}


namespace game
{

class ObjectVisualAttribute : public sgl::sgui::Widget
{
public:
    ObjectVisualAttribute(sgl::sgui::Widget * parent);

    void ClearData();
    void SetData(const char * txt, unsigned int val);

    void HandlePositionChanged() override;

private:
    sgl::graphic::Image * mBg = nullptr;
    sgl::sgui::Label * mLabel = nullptr;
    sgl::sgui::Image * mValueBar = nullptr;
};

} // namespace game
