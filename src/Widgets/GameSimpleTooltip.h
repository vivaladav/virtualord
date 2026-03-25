#pragma once

#include <sgl/sgui/Widget.h>

namespace sgl
{
    namespace graphic
    {
        class Image;
        class Text;
    }
}

namespace game
{

class Game;

class GameSimpleTooltip : public sgl::sgui::Widget
{
public:
    GameSimpleTooltip(const char * text);

    void SetText(const char * text);

protected:
    void HandlePositionChanged() override;

private:
    void SetPositions();

private:
    sgl::graphic::Image * mBgL = nullptr;
    sgl::graphic::Image * mBgC = nullptr;
    sgl::graphic::Image * mBgR = nullptr;

    sgl::graphic::Text * mLabel = nullptr;
};

} // namespace game
