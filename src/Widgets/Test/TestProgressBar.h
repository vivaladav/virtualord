#pragma once

#include <sgl/sgui/ProgressBar.h>

namespace sgl { namespace graphic { class Image; } }

namespace game
{

enum PlayerFaction : unsigned int;

class TestProgressBar : public sgl::sgui::ProgressBar
{
public:
    TestProgressBar(PlayerFaction faction, float min, float max, sgl::sgui::Widget * parent = nullptr);

private:
    void HandlePositionChanged() override;

    void HandleProgressUpdate() override;

private:
    sgl::graphic::Image * mBg = nullptr;
    sgl::graphic::Image * mBar = nullptr;

    int mBarW;
    int mBarH;
};

} // namespace game
