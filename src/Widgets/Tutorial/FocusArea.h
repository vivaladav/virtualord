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

    void SetScreenArea(int x0, int y0, int w, int h);
    void SetWorldArea(int x0, int y0, int w, int h);

    void SetCornersColor(unsigned int color);

private:
    void SetArea(int x0, int y0, int w, int h);

private:
    sgl::graphic::Image * mCornerTL = nullptr;
    sgl::graphic::Image * mCornerTR = nullptr;
    sgl::graphic::Image * mCornerBL = nullptr;
    sgl::graphic::Image * mCornerBR = nullptr;
};

} // namespace game
