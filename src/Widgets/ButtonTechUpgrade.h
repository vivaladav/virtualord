#pragma once

#include <sgl/sgui/AbstractButton.h>

#include <unordered_map>

namespace sgl
{
    namespace graphic
    {
        class Image;
        class Texture;
    }
}

namespace game
{

enum TechUpgradeId : unsigned int;

class ButtonTechUpgrade : public sgl::sgui::AbstractButton
{
public:
    ButtonTechUpgrade(TechUpgradeId upgrade, sgl::sgui::Widget * parent);
    ~ButtonTechUpgrade();

    void SetUpgrade(TechUpgradeId upgrade);

private:
    void HandleMouseOver() override;

    void HandleButtonDown() override;

    void OnStateChanged(sgl::sgui::AbstractButton::VisualState state) override;

    void UpdateGraphics(sgl::sgui::AbstractButton::VisualState state);

    void HandlePositionChanged() override;
    void UpdatePositions();

private:
    std::array<sgl::graphic::Texture *, sgl::sgui::AbstractButton::NUM_VISUAL_STATES> mTexs;

    std::unordered_map<unsigned int, unsigned int> mIconsIds;

    sgl::graphic::Image * mBg = nullptr;
    sgl::graphic::Image * mIcon = nullptr;

    bool mIconVisible = false;
};

} // namespace game
