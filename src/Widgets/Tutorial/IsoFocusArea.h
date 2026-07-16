#pragma once

#include <sgl/core/Point.h>
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

    void SetCell(int r0, int c0, bool anim = true, float delayAnim = 0.5f);
    void SetCellArea(int r0, int c0, int r1, int c1, bool anim = true, float delayAnim = 0.5f);

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

    sgl::core::Pointf2D mCornerPosTL;
    sgl::core::Pointf2D mCornerPosTR;
    sgl::core::Pointf2D mCornerPosBL;
    sgl::core::Pointf2D mCornerPosBR;

    float mTimerBlinking = 0.f;
    float mAnimationMove = 0.f;
    float mAnimationDelay = 0.f;

    bool mBlinking = false;
    bool mRendering = true;
    bool mAnimating = false;
};

inline void IsoFocusArea::SetCell(int r0, int c0, bool anim, float delayAnim)
{
    SetCellArea(r0, c0, r0, c0, anim, delayAnim);
}

} // namespace game
