#pragma once

#include <sgl/sgui/Widget.h>

namespace sgl
{
    namespace graphic { class Image; }

    namespace sgui { class Label; }
}

namespace game
{

class PanelUnitResourcesUsage : public sgl::sgui::Widget
{
public:
    PanelUnitResourcesUsage();

    void SetValues(int unitEnergy, int resEnergy, int resMaterial);
    void SetDoable(bool doable);

private:
    void HandlePositionChanged() override;

    void UpdateGraphics();
    void UpdatePositions();

private:
    sgl::graphic::Image * mBg = nullptr;

    sgl::sgui::Label * mLabelUnitEnergy = nullptr;
    sgl::sgui::Label * mLabelResEnergy = nullptr;
    sgl::sgui::Label * mLabelResMaterial = nullptr;

    bool mDoable = true;
};

} // namespace game
