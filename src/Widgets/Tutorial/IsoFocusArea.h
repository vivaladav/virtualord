#pragma once

#include <sgl/sgui/Widget.h>

namespace sgl
{
    namespace graphic { class Image; }
}

namespace game
{

class IsoMap;

class IsoFocusArea : public sgl::sgui::Widget
{
public:
    IsoFocusArea(const IsoMap * im);

    void SetBlinking(bool enabled);

    void SetCell(int r0, int c0);
    void SetCellArea(int r0, int c0, int r1, int c1);

    void SetCornersColor(unsigned int color);
    void SetCornersColorElement();
    void SetCornersColorAction();

private:
     void OnRender() override;
    void OnUpdate(float delta) override;

private:
    const IsoMap * mIM = nullptr;

    sgl::graphic::Image * mCornerTL = nullptr;
    sgl::graphic::Image * mCornerTR = nullptr;
    sgl::graphic::Image * mCornerBL = nullptr;
    sgl::graphic::Image * mCornerBR = nullptr;

    float mTimerBlinking = 0.f;
    bool mBlinking = false;
    bool mBlinkOn = true;
};

inline void IsoFocusArea::SetCell(int r0, int c0)
{
    SetCellArea(r0, c0, r0, c0);
}

} // namespace game
