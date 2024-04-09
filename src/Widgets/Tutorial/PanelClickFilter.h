#pragma once

#include <sgl/sgui/Widget.h>

namespace game
{

class PanelClickFilter : public sgl::sgui::Widget
{
public:
    PanelClickFilter();

    void SetScreenClickableArea(int x0, int y0, int w, int h);
    void SetWorldClickableArea(int x0, int y0, int w, int h);

private:
    void SetClickableArea(int x0, int y0, int w, int h);

    void HandleMouseButtonDown(sgl::core::MouseButtonEvent & event) override;
    void HandleMouseButtonUp(sgl::core::MouseButtonEvent & event) override;

    void FilterMouseEvent(sgl::core::MouseButtonEvent & event);

private:
    int mXtl = 0;
    int mYtl = 0;
    int mXbr = 0;
    int mYbr = 0;

    bool mAreaWorld = false;
};

inline void PanelClickFilter::SetClickableArea(int x0, int y0, int w, int h)
{
    mXtl = x0;
    mYtl = y0;
    mXbr = x0 + w;
    mYbr = y0 + h;
}

inline void PanelClickFilter::SetScreenClickableArea(int x0, int y0, int w, int h)
{
    mAreaWorld = false;

    SetClickableArea(x0, y0, w, h);
}

inline void PanelClickFilter::SetWorldClickableArea(int x0, int y0, int w, int h)
{
    mAreaWorld = true;

    SetClickableArea(x0, y0, w, h);
}

} // namespace game
