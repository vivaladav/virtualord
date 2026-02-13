#pragma once

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

class Player;
class ResearchCenter;

class DialogResearch : public sgl::sgui::Widget
{
public:
    DialogResearch(Player * player, ResearchCenter * rc);

    void SetFunctionOnClose(const std::function<void()> & f);

private:
    void HandlePositionChanged() override;

    void OnInvestmentChanged();

    void SetPositions();

    void UpdateOutput();

private:
    sgl::graphic::Image * mBgL = nullptr;
    sgl::graphic::Image * mBgC = nullptr;
    sgl::graphic::Image * mBgR = nullptr;

    sgl::sgui::AbstractButton * mBtnClose = nullptr;

    sgl::sgui::Label * mLabelProd = nullptr;

    Player * mPlayer = nullptr;
    ResearchCenter * mResCenter = nullptr;
};

} // namespace game
