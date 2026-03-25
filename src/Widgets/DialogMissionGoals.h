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

class MissionGoalsTracker;

class DialogMissionGoals : public sgl::sgui::Widget
{
public:
    DialogMissionGoals(MissionGoalsTracker * mgt);

    const sgl::sgui::AbstractButton * GetButtonClose() const;
    const sgl::sgui::AbstractButton * GetButtonEnd() const;

    unsigned int AddFunctionOnClose(const std::function<void()> & f);
    void RemoveFunctionOnClose(unsigned int funId);
    unsigned int AddFunctionOnEnd(const std::function<void()> & f);
    void RemoveFunctionOnEnd(unsigned int funId);

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

    MissionGoalsTracker * mTrackerMG = nullptr;
};

inline const sgl::sgui::AbstractButton * DialogMissionGoals::GetButtonClose() const
{
    return mBtnClose;
}

inline const sgl::sgui::AbstractButton * DialogMissionGoals::GetButtonEnd() const
{
    return mBtnEnd;
}


} // namespace game
