#pragma once

#include <sgl/sgui/Widget.h>

#include <functional>

namespace sgl
{
    namespace graphic
    {
        class Image;
        class Texture;
    }

    namespace sgui { class AbstractButton; }
}

namespace game
{

class ScreenGame;

class DialogMissionGoals : public sgl::sgui::Widget
{
public:
    DialogMissionGoals(ScreenGame * screen);

    void SetFunctionOnClose(const std::function<void()> & f);
    void SetFunctionOnEnd(const std::function<void()> & f);

private:
    sgl::sgui::Widget * CreateGoalEntry(unsigned int goalInd, sgl::graphic::Texture * texBg);

    void CheckIfEndAllowed();

    void HandlePositionChanged() override;

    void SetPositions();

private:
    sgl::graphic::Image * mBgTop = nullptr;
    sgl::graphic::Image * mBgMid = nullptr;
    sgl::graphic::Image * mBgBot = nullptr;

    sgl::sgui::AbstractButton * mBtnClose = nullptr;
    sgl::sgui::AbstractButton * mBtnEnd = nullptr;

    ScreenGame * mScreen = nullptr;
};

} // namespace game
