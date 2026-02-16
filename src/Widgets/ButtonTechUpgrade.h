#pragma once

#include <sgl/sgui/AbstractButton.h>


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

class ButtonTechUpgrade : public sgl::sgui::AbstractButton
{
public:
    ButtonTechUpgrade(sgl::sgui::Widget * parent);

private:
    void HandleMouseOver() override;

    void HandleButtonDown() override;

    void OnStateChanged(sgl::sgui::AbstractButton::VisualState state) override;

    void UpdateGraphics(sgl::sgui::AbstractButton::VisualState state);

    void HandlePositionChanged() override;
    void UpdatePositions();

private:
    std::array<sgl::graphic::Texture *, sgl::sgui::AbstractButton::NUM_VISUAL_STATES> mTexs;

    sgl::graphic::Image * mBg = nullptr;
};

} // namespace game
