#pragma once

#include <sgl/sgui/Widget.h>

namespace sgl
{
    namespace graphic
    {
        class Image;
        class Text;
    }

    namespace sgui { class Label; }
}

namespace game
{

class Game;

class ResourceTooltip : public sgl::sgui::Widget
{
public:
    ResourceTooltip(const char * title);

    void SetValues(int resIn, int resOut);

protected:
    void HandlePositionChanged() override;

private:
    void SetPositions();

private:
    sgl::graphic::Image * mBg = nullptr;
    sgl::graphic::Text * mTitle = nullptr;
    sgl::sgui::Label * mLabelIn = nullptr;
    sgl::sgui::Label * mLabelOut = nullptr;
    sgl::sgui::Label * mLabelTot = nullptr;
};

} // namespace game
