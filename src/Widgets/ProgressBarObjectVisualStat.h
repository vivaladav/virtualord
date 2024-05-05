#pragma once

#include <sgl/sgui/ProgressBar.h>

namespace sgl { namespace graphic { class Image; } }

namespace game
{

enum PlayerFaction : unsigned int;

class ProgressBarObjectVisualStat : public sgl::sgui::ProgressBar
{
public:
    ProgressBarObjectVisualStat(float min, float max, sgl::sgui::Widget * parent);

private:
    void HandlePositionChanged() override;

    void HandleProgressUpdate() override;

private:
    sgl::graphic::Image * mBg = nullptr;
    sgl::graphic::Image * mBar = nullptr;
    sgl::graphic::Image * mMarks = nullptr;

    int mBarW;
    int mBarH;
};

} // namespace game
