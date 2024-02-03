#pragma once

#include <sgl/sgui/ProgressBar.h>

namespace sgl { namespace graphic { class Image; } }

namespace game
{

enum PlayerFaction : unsigned int;

class ProgressBarTurnEnergy : public sgl::sgui::ProgressBar
{
public:
    ProgressBarTurnEnergy(PlayerFaction faction, float min, float max, sgl::sgui::Widget * parent);

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
