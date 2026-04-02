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

class PanelUnitResourcesUsage : public sgl::sgui::Widget
{
public:
    PanelUnitResourcesUsage();

    void SetValue(int val);
    void SetDoable(bool doable);

private:
    void HandlePositionChanged() override;

    void UpdateGraphics();
    void UpdatePositions();

private:
    sgl::graphic::Image * mBg = nullptr;

    bool mDoable = true;
};

} // namespace game
