#pragma once

#include <sgl/sgui/Widget.h>

namespace sgl
{
    namespace graphic
    {
        class Image;
        class Renderable;
    }
}

namespace game
{

class WarningMessage : public sgl::sgui::Widget
{
public:
    WarningMessage();

    void ShowMessage(const char * text, float time);

    void FadeOut();

private:
    void HandlePositionChanged() override;

    void UpdatePositions();

    void OnUpdate(float delta) override;

private:
    sgl::graphic::Image * mBgL = nullptr;
    sgl::graphic::Image * mBgC = nullptr;
    sgl::graphic::Image * mBgR = nullptr;

    sgl::graphic::Renderable * mText = nullptr;

    float mTimer = 0.f;
};

} // namespace game
