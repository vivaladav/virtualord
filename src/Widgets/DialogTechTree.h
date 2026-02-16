#pragma once

#include <sgl/sgui/Widget.h>

#include <functional>
#include <vector>

namespace sgl
{
    namespace graphic { class Image; }

    namespace sgui
    {
        class AbstractButton;
        class ButtonsGroup;
    }
}

namespace game
{

class ButtonTechUpgrade;
class Player;
class Screen;

class DialogTechTree : public sgl::sgui::Widget
{
public:
    DialogTechTree(Player * player);

    void SetFunctionOnClose(const std::function<void()> & f);

private:
    void HandlePositionChanged() override;

    void SetPositions();

    void UpdateUpgrades();

private:
    std::vector<ButtonTechUpgrade *> mUpgrades;

    sgl::graphic::Image * mBgL = nullptr;
    sgl::graphic::Image * mBgC = nullptr;
    sgl::graphic::Image * mBgR = nullptr;

    sgl::sgui::AbstractButton * mBtnClose = nullptr;

    sgl::sgui::ButtonsGroup * mButtonsSection = nullptr;

    Player * mPlayer = nullptr;
};

} // namespace game
