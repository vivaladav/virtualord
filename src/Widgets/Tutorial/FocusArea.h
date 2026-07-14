#pragma once

#include <sgl/core/Point.h>
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
    void SetCornersColorElement();
    void SetCornersColorAction();

private:
    void SetArea(int x0, int y0, int w, int h);

    void OnRender() override;
    void OnUpdate(float delta) override;

private:
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

} // namespace game
