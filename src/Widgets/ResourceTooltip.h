#pragma once

#include <sgl/sgui/Widget.h>

#include <string>

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

    void SetValues(unsigned int resIn, unsigned int resOut);

protected:
    void HandlePositionChanged() override;

private:
    void SetPositions();

    std::string GetSignedNumber(int val);

private:
    sgl::graphic::Image * mBg = nullptr;
    sgl::graphic::Text * mTitle = nullptr;
    sgl::sgui::Label * mLabelIn = nullptr;
    sgl::sgui::Label * mLabelOut = nullptr;
    sgl::sgui::Label * mLabelTot = nullptr;

    unsigned int mIn = -1;
    unsigned int mOut = -1;
};

} // namespace game
