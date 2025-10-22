#pragma once

#include "MissionGoal.h"

#include <sgl/sgui/Widget.h>

#include <functional>

namespace sgl
{
    namespace graphic
    {
        class Image;
        class Texture;
    }
    namespace sgui
    {
        class AbstractButton;
        class Label;
    }
}

namespace game
{

class ScreenGame;

class DialogMissionGoals : public sgl::sgui::Widget
{
public:
    DialogMissionGoals(const std::vector<MissionGoal> & goals, ScreenGame * screen);

    void SetFunctionOnClose(const std::function<void()> & f);

private:
    sgl::sgui::Widget * CreateGoalEntry(unsigned int goalInd, sgl::graphic::Texture * texBg);

    void HandlePositionChanged() override;

    void SetPositions();

private:
    std::vector<MissionGoal> mGoals;

    sgl::graphic::Image * mBgTop = nullptr;
    sgl::graphic::Image * mBgMid = nullptr;
    sgl::graphic::Image * mBgBot = nullptr;

    sgl::sgui::AbstractButton * mBtnClose = nullptr;
    sgl::sgui::AbstractButton * mBtnEnd = nullptr;

    sgl::sgui::Label * mTitle = nullptr;

    ScreenGame * mScreen = nullptr;
};

} // namespace game
