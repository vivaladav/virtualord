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
class ObjectVisualStat;

class PanelSelectedObject : public sgl::sgui::Widget
{

public:
    PanelSelectedObject(const ObjectsDataRegistry * odr, sgl::sgui::Widget * parent);

    void AddFunctionOnClose(const std::function<void()> & f);
    void AddFunctionOnShowInfo(const std::function<void()> & f);

    void SetObject(GameObject * obj);

private:
    void HandlePositionChanged() override;
    void PositionElements();

    void UpdateStats();

private:
    const ObjectsDataRegistry * mObjDataReg;

    sgl::graphic::Image * mBg = nullptr;

    sgl::sgui::ImageButton * mButtonClose = nullptr;

    sgl::graphic::Image * mImg = nullptr;
    sgl::sgui::TextArea * mTitle = nullptr;
    sgl::sgui::Image * mBarLvl = nullptr;

    ObjectVisualStat * mStatEnergy = nullptr;
    ObjectVisualStat * mStatHealth = nullptr;
    ObjectVisualStat * mStatExperience = nullptr;
    unsigned int mFuncValuesChangedId = 0;

    // buttons function
    sgl::sgui::ImageButton * mButtonAutoAttack = nullptr;
    sgl::sgui::ImageButton * mButtonAutoMove = nullptr;
    sgl::sgui::ImageButton * mButtonShowInfo = nullptr;
    sgl::sgui::ImageButton * mButtonShowUpgrade = nullptr;

    GameObject * mObj = nullptr;
};

} // namespace game
