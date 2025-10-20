#pragma once

#include "MissionGoal.h"

#include <sgl/sgui/Widget.h>

#include <functional>

namespace sgl
{
    namespace graphic { class Image; }
    namespace sgui
    {
        class AbstractButton;
        class Label;
    }
}

namespace game
{

class DialogMissionGoals : public sgl::sgui::Widget
{
public:
    DialogMissionGoals(const std::vector<MissionGoal> & goals);

    void SetFunctionOnClose(const std::function<void()> & f);

protected:
    void HandlePositionChanged() override;

private:
    void SetPositions();

private:
    std::vector<MissionGoal> mGoals;

    sgl::graphic::Image * mBgTop = nullptr;
    sgl::graphic::Image * mBgMid = nullptr;
    sgl::graphic::Image * mBgBot = nullptr;

    sgl::sgui::AbstractButton * mBtnClose = nullptr;
    sgl::sgui::AbstractButton * mBtnEnd = nullptr;

    sgl::sgui::Label * mTitle = nullptr;
};

} // namespace game
