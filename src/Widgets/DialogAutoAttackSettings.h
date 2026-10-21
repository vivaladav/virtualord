#pragma once

#include <sgl/sgui/Widget.h>

#include <functional>

namespace sgl
{
    namespace graphic { class Image; }

    namespace sgui
    {
        class AbstractButton;
        class Slider;
    }
}

namespace game
{

class GameComboBox;
class GameObject;

class DialogAutoAttackSettings : public sgl::sgui::Widget
{
public:
    DialogAutoAttackSettings(GameObject * obj);

    void SetFunctionOnClose(const std::function<void()> & f);

private:
    void HandlePositionChanged() override;

    void OnInvestmentChanged();

    void SetPositions();

private:
    GameObject * mObj = nullptr;

    sgl::graphic::Image * mBg = nullptr;

    GameComboBox * mComboEnemySel = nullptr;
    GameComboBox * mComboAttackMode = nullptr;

    sgl::sgui::Slider * mSliderEnergy = nullptr;

    sgl::sgui::AbstractButton * mBtnClose = nullptr;
};

} // namespace game
