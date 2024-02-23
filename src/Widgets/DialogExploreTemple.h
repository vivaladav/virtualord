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
        class Label;
        class TextArea;
    }
}

namespace game
{

class Game;
class GameButton;
class GameSliderH;
class Player;
class Screen;
class Temple;

// == DIALOG EXPLORE TEMPLE ==
class DialogExploreTemple : public sgl::sgui::Widget
{
public:
    DialogExploreTemple(Player * player, Temple * temple);

    void SetFunctionOnCancel(const std::function<void()> & f);
    void SetFunctionOnExplore(const std::function<void()> & f);

protected:
    void HandlePositionChanged() override;

    void OnInvestmentChanged();

private:
    void SetPositions();

private:
    sgl::graphic::Image * mBg = nullptr;
    sgl::graphic::Image * mLineH1 = nullptr;
    sgl::graphic::Image * mLineH2 = nullptr;

    sgl::graphic::Text * mHeaderInvest = nullptr;
    sgl::graphic::Text * mHeaderTurns = nullptr;
    sgl::graphic::Text * mHeaderSuccess = nullptr;
    sgl::sgui::Label * mLabelTurns = nullptr;
    sgl::sgui::Label * mLabelSuccess = nullptr;

    GameButton * mBtnAbort = nullptr;
    GameButton * mBtnExplore = nullptr;

    GameSliderH * mSliderMoney = nullptr;
    GameSliderH * mSliderMaterial = nullptr;
    GameSliderH * mSliderBlobs = nullptr;
    GameSliderH * mSliderDiamonds = nullptr;

    Player * mPlayer = nullptr;
    Temple * mTemple = nullptr;
};

// == DIALOG EXPLORE TEMPLE OUTCOME ==
class DialogExploreTempleOutcome : public sgl::sgui::Widget
{
public:
    DialogExploreTempleOutcome(Player * player, Temple * temple);

    void SetFunctionOnClose(const std::function<void()> & f);
    void SetFunctionOnOutcome1(const std::function<void()> & f);
    void SetFunctionOnOutcome2(const std::function<void()> & f);

protected:
    void HandlePositionChanged() override;

private:
    void SetPositions();

private:
    sgl::graphic::Image * mBg = nullptr;
    sgl::graphic::Image * mLine = nullptr;

    GameButton * mBtnClose = nullptr;
    GameButton * mBtnOutcome1 = nullptr;
    GameButton * mBtnOutcome2 = nullptr;

    sgl::graphic::Text * mHeaderOutc1 = nullptr;
    sgl::graphic::Text * mHeaderOutc2 = nullptr;

    sgl::sgui::TextArea * mDescOutc1 = nullptr;
    sgl::sgui::TextArea * mDescOutc2 = nullptr;

    Player * mPlayer = nullptr;
    Temple * mTemple = nullptr;
};

} // namespace game
