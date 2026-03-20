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

enum ExtendedResource : unsigned int;

class ResourceTooltip : public sgl::sgui::Widget
{
public:
    ResourceTooltip(ExtendedResource res);

    void UpdateStrings();

    void SetValues(int val, int maxVal, int resIn, int resOut);

protected:
    void HandlePositionChanged() override;

private:
    void SetPositions();

    std::string GetSignedNumber(int val);

private:
    sgl::graphic::Image * mBg = nullptr;
    sgl::sgui::Label * mHeader1 = nullptr;
    sgl::sgui::Label * mHeader2 = nullptr;
    sgl::sgui::Label * mLabelVal = nullptr;
    sgl::sgui::Label * mLabelIn = nullptr;
    sgl::sgui::Label * mLabelOut = nullptr;
    sgl::sgui::Label * mLabelTot = nullptr;

    ExtendedResource mRes;

    unsigned int mIn = -1;
    unsigned int mOut = -1;
};

} // namespace game
