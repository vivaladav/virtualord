#pragma once

#include <sgl/sgui/Widget.h>

#include <array>

namespace sgl
{
    namespace graphic { class Image; }
}

namespace game
{

class PanelInfoTutorial : public sgl::sgui::Widget
{
public:
    PanelInfoTutorial(int w, int h);

private:
    void HandlePositionChanged() override;

    void PositionElements();

private:
    enum BgParts : unsigned int
    {
        BGPART_TL,
        BGPART_TR,
        BGPART_BL,
        BGPART_BR,
        BGPART_CENTER,
        BGPART_L,
        BGPART_R,
        BGPART_T,
        BGPART_B,

        NUM_BGPARTS
    };

    std::array<sgl::graphic::Image *, NUM_BGPARTS> mBgParts;
};

} // namespace game
