#pragma once

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

class GameObject;

class PanelUnitEnergyUsage : public sgl::sgui::Widget
{
public:
    PanelUnitEnergyUsage();

    void SetValue(int val);

private:
    void HandlePositionChanged() override;

    void UpdatePositions();

private:
    sgl::graphic::Image * mBg = nullptr;

    sgl::sgui::Label * mLabel = nullptr;
    sgl::sgui::Image * mIcon = nullptr;
};

} // namespace game
