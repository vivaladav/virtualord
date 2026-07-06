#pragma once

#include <sgl/sgui/Widget.h>
#include <sgl/utilities/StringsChangeListener.h>

namespace sgl
{
    namespace graphic { class Image; }

    namespace sgui { class Label; }
}

namespace game
{

class PanelUnitResourcesUsage : public sgl::sgui::Widget,
                                public sgl::utilities::StringsChangeListener
{
public:
    PanelUnitResourcesUsage();

    void SetValues(int unitEnergy, int resEnergy, int resMaterial);
    void SetDoable(bool unitEnergy, bool resEnergy, bool resMaterial);
    bool IsDoable() const;
    bool IsDoableUnit() const;
    bool IsDoableResources() const;

private:
    void HandlePositionChanged() override;

    void UpdateGraphics();
    void UpdatePositions();

    void OnStringsChanged() override;

private:
    sgl::graphic::Image * mBg = nullptr;

    sgl::sgui::Label * mHeaderUnit = nullptr;
    sgl::sgui::Label * mHeaderRes = nullptr;

    sgl::sgui::Label * mLabelUnitEnergy = nullptr;
    sgl::sgui::Label * mLabelResEnergy = nullptr;
    sgl::sgui::Label * mLabelResMaterial = nullptr;

    bool mDoableUnitEnergy = true;
    bool mDoableResEnergy = true;
    bool mDoableResMaterial = true;
};

inline bool PanelUnitResourcesUsage::IsDoable() const
{
    return mDoableUnitEnergy && mDoableResEnergy && mDoableResMaterial;
}
inline bool PanelUnitResourcesUsage::IsDoableUnit() const { return mDoableUnitEnergy; }
inline bool PanelUnitResourcesUsage::IsDoableResources() const
{
    return mDoableResEnergy && mDoableResMaterial;
}

} // namespace game
