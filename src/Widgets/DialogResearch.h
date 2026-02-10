#pragma once

#include <sgl/sgui/Widget.h>

#include <functional>

namespace sgl
{
    namespace graphic
    {
        class Image;
        class Text;
    }

    namespace sgui
    {
        class AbstractButton;
        class Label;
        class TextArea;
    }
}

namespace game
{

class Game;
class GameSliderH;
class Player;
class ResearchCenter;
class Screen;

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

    sgl::graphic::Text * mHeaderInvest = nullptr;
    sgl::graphic::Text * mHeaderTurns = nullptr;

    sgl::sgui::AbstractButton * mBtnClose = nullptr;

    GameSliderH * mSliderMoney = nullptr;
    GameSliderH * mSliderEnergy = nullptr;
    GameSliderH * mSliderMaterial = nullptr;
    GameSliderH * mSliderBlobs = nullptr;
    GameSliderH * mSliderDiamonds = nullptr;

    sgl::sgui::Label * mLabelProd = nullptr;

    Player * mPlayer = nullptr;
    ResearchCenter * mResCenter = nullptr;
};

} // namespace game
