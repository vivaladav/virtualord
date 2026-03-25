#pragma once

#include <sgl/sgui/ImageButton.h>

#include <functional>
#include <string>
#include <vector>

namespace sgl
{
    namespace graphic { class Image; }

    namespace sgui
    {
        class ButtonsGroup;
        class Image;
        class Label;
    }
}

namespace game
{

// ===== DIALOG TECH TREE  =====
class ButtonUnlock;
class ButtonTechUpgrade;
class Game;
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

    enum LinkType : unsigned int
    {
        LINK_VERT,
        LINK_HORIZ,

        NUM_LINK_TYPES
    };

    enum LinkSlot : unsigned int
    {
        LS_NORTH,
        LS_SOUTH,
        LS_WEST,
        LS_EAST,

        NUM_LINK_SLOTS
    };

private:
    void HandlePositionChanged() override;

    void SetPositions();

    void UpdateUpgrades(UpgradeSections section);

    void ClearButtonsUpgrade();
    ButtonTechUpgrade * GetNewButtonUpgrade(TechUpgradeId upgrade, int level,
                                            const std::vector<ButtonTechUpgrade *> & enablers,
                                            bool enabled);

    void ClearLinks();
    sgl::sgui::Image * GetNewLink(unsigned int texID);
    void AddLinkToUpgrade(ButtonTechUpgrade * btn, LinkType type, LinkSlot slot);

    void SetDescription(TechUpgradeId upgrade);

private:
    std::vector<ButtonTechUpgrade *> mButtonsUpgrade;
    std::vector<sgl::sgui::Image *> mLinks;
    std::unordered_map<TechUpgradeId, std::string> mDescriptions;
    std::unordered_map<TechUpgradeId, int> mCosts;

    sgl::graphic::Image * mBgL = nullptr;
    sgl::graphic::Image * mBgC = nullptr;
    sgl::graphic::Image * mBgR = nullptr;

    sgl::sgui::AbstractButton * mBtnClose = nullptr;

    sgl::sgui::ButtonsGroup * mButtonsSection = nullptr;

    sgl::sgui::Label * mLabelDescription = nullptr;
    sgl::sgui::Label * mLabelUnlocked = nullptr;

    ButtonUnlock * mBtnUnlock = nullptr;

    Player * mPlayer = nullptr;

    unsigned int mButtonsUpgradeUsed = 0;
    unsigned int mLinksUsed = 0;
};

// ===== BUTTON UNLOCK  =====
class ButtonUnlock : public sgl::sgui::ImageButton
{
public:
    ButtonUnlock(sgl::sgui::Widget * parent);

    void SetCost(int cost);

    void ClearUpgradeToUnlock();
    ButtonTechUpgrade * GetUpgradeToUnlock() const;
    void SetUpgradeToUnlock(ButtonTechUpgrade * btn);

private:
    void OnStateChanged(sgl::sgui::AbstractButton::VisualState state) override;

    void UpdateGraphics();

    void HandlePositionChanged() override;
    void UpdatePositions();

    void HandleMouseOver() override;
    void HandleButtonDown() override;

private:
    sgl::sgui::Label * mLabel = nullptr;
    sgl::sgui::Image * mIcon = nullptr;

    ButtonTechUpgrade * mUpgradeToUnlock = nullptr;
};

inline void ButtonUnlock::ClearUpgradeToUnlock() { mUpgradeToUnlock = nullptr; }
inline ButtonTechUpgrade * ButtonUnlock::GetUpgradeToUnlock() const { return mUpgradeToUnlock; }
inline void ButtonUnlock::SetUpgradeToUnlock(ButtonTechUpgrade * btn) { mUpgradeToUnlock = btn; }


} // namespace game
