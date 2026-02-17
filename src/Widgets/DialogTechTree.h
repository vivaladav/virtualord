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

enum TechUpgradeId : unsigned int;

class DialogTechTree : public sgl::sgui::Widget
{
public:
    DialogTechTree(Player * player);

    void SetFunctionOnClose(const std::function<void()> & f);

private:
    enum UpgradeSections : unsigned int
    {
        SEC_STRUCTURES,
        SEC_UNITS,
        SEC_RESOURCES,
        SEC_TECHNOLOGY,
        SEC_SPECIALS,

        NUM_UPG_SECTIONS
    };

    void HandlePositionChanged() override;

    void SetPositions();

    void UpdateUpgrades(UpgradeSections section);

    void ClearButtonsUpgrade();
    ButtonTechUpgrade * GetNewButtonUpgrade(TechUpgradeId upgrade, bool enabled, bool unlocked);

private:
    std::vector<ButtonTechUpgrade *> mButtonsUpgrade;

    sgl::graphic::Image * mBgL = nullptr;
    sgl::graphic::Image * mBgC = nullptr;
    sgl::graphic::Image * mBgR = nullptr;

    sgl::sgui::AbstractButton * mBtnClose = nullptr;

    sgl::sgui::ButtonsGroup * mButtonsSection = nullptr;

    Player * mPlayer = nullptr;

    unsigned int mButtonsUpgradeUsed = 0;
};

} // namespace game
