#pragma once

#include <sgl/sgui/Slider.h>

namespace sgl
{
    namespace graphic { class Texture; }
}

namespace game
{


// ==== COMBO BOX ====
class GameSliderH : public sgl::sgui::Slider
{

public:
    GameSliderH(const char * fileSprite, unsigned int texBgID, unsigned int  texBarID,
                unsigned int texButtonID, sgl::sgui::Widget * parent);
    GameSliderH(sgl::graphic::Texture * texBg, sgl::graphic::Texture * texBar,
                sgl::graphic::Texture * texButton, sgl::sgui::Widget * parent);

private:
    void OnStateChanged(sgl::sgui::AbstractSlider::VisualState state) override;

    void HandlePositionChanged() override;

    void HandleValueChanged(int val) override;

    void UpdateGraphics(sgl::sgui::AbstractSlider::VisualState state);
    void UpdatePositions();

private:
    sgl::graphic::Texture * mTexBar = nullptr;
};

} // namespace game
