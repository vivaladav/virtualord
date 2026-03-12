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

class ButtonDialogClose;
class GameObject;
class ObjectsDataRegistry;

class ValueUpgradeBar : public sgl::sgui::Widget
{
public:
    ValueUpgradeBar(int maxVal, sgl::sgui::Widget * parent);

    void SetValue(int val);

    void AddNew();
    void RemNew();

private:
    std::vector<sgl::sgui::Image *> mImgs;

    unsigned int mValue = 0;
    unsigned int mNumNew = 0;
};

class DialogUpgrade : public sgl::sgui::Widget
{
public:
    DialogUpgrade(GameObject * obj, const ObjectsDataRegistry * odr);

    void SetFunctionOnClose(const std::function<void()> & f);

    void SetObject(GameObject * obj);

private:
    void HandlePositionChanged() override;

    void SetPositions();

    void OnPointsChanged();

private:
    std::vector<int> mChangesToApply;

    std::vector<sgl::sgui::AbstractButton *> mButtonsDec;
    std::vector<sgl::sgui::AbstractButton *> mButtonsInc;

    sgl::graphic::Image * mBgL = nullptr;
    sgl::graphic::Image * mBgC = nullptr;
    sgl::graphic::Image * mBgR = nullptr;

    sgl::sgui::AbstractButton * mBtnClose = nullptr;
    sgl::sgui::AbstractButton * mBtnUpgrade = nullptr;

    sgl::sgui::Label * mLabelPoints = nullptr;

    GameObject * mObj = nullptr;

    int mPointsToAssign = 0;
};

} // namespace game
