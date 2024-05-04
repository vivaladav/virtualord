#pragma once

#include <sgl/sgui/Widget.h>

namespace sgl
{
    namespace graphic { class Image; }
}

namespace game
{

class FocusArea : public sgl::sgui::Widget
{
public:
    FocusArea();

    void SetBlinking(bool enabled);

    void SetScreenArea(int x0, int y0, int w, int h);
    void SetWorldArea(int x0, int y0, int w, int h);

    void SetCornersColor(unsigned int color);

private:
    void SetArea(int x0, int y0, int w, int h);

    void OnRender() override;
    void OnUpdate(float delta) override;

private:
    sgl::graphic::Image * mCornerTL = nullptr;
    sgl::graphic::Image * mCornerTR = nullptr;
    sgl::graphic::Image * mCornerBL = nullptr;
    sgl::graphic::Image * mCornerBR = nullptr;

    float mTimerBlinking = 0.f;
    bool mBlinking = false;
    bool mBlinkOn = true;

};

} // namespace game
