#pragma once

#include <sgl/sgui/AbstractButton.h>

#include <unordered_map>
#include <vector>

namespace sgl
{
    namespace graphic
    {
        class Image;
        class Text;
        class Texture;
    }

    namespace sgui { class Image; }
}

namespace game
{

enum TechUpgradeId : unsigned int;

class ButtonTechUpgrade : public sgl::sgui::AbstractButton
{
public:
    ButtonTechUpgrade(TechUpgradeId upgrade, sgl::sgui::Widget * parent);
    ~ButtonTechUpgrade();

    TechUpgradeId GetUpgrade() const;
    void SetUpgrade(TechUpgradeId upgrade);

    unsigned int GetLevel() const;
    void SetLevel(unsigned int lvl);

    bool IsUnlocked() const;
    void SetUnlocked(bool unlocked);

    void ClearLinks();
    void AddLink(sgl::sgui::Image * link);

    void ClearButtonsToEnable();
    void AddButtonToEnable(ButtonTechUpgrade * b);

    void SetOnMouseOver(const std::function<void()> & f);
    void SetOnMouseOut(const std::function<void()> & f);

private:
    void HandleMouseOver() override;
    void HandleMouseOut() override;

    void HandleButtonDown() override;

    void OnStateChanged(sgl::sgui::AbstractButton::VisualState state) override;

    void UpdateGraphics(sgl::sgui::AbstractButton::VisualState state);
    void UpdateColorsIcon();
    void UpdateColorsLevel();
    void UpdateColorLink(sgl::sgui::Image * link);

    void HandlePositionChanged() override;
    void UpdatePositions();

private:
    std::array<sgl::graphic::Texture *, sgl::sgui::AbstractButton::NUM_VISUAL_STATES> mTexs;
    std::unordered_map<TechUpgradeId, unsigned int> mIconsIds;
    std::vector<sgl::sgui::Image *> mLinks;
    std::vector<ButtonTechUpgrade *> mButtonsToEnable;

    std::function<void()> mFuncOnMouseOver;
    std::function<void()> mFuncOnMouseOut;

    sgl::graphic::Image * mBg = nullptr;
    sgl::graphic::Image * mIcon = nullptr;

    sgl::graphic::Image * mBgLevel = nullptr;
    sgl::graphic::Text * mLabelLevel = nullptr;

    TechUpgradeId mUpgrade;

    unsigned int mLevel = 0;
    bool mLevelVisible = false;
    bool mIconVisible = false;
    bool mUnlocked = false;
};

inline TechUpgradeId ButtonTechUpgrade::GetUpgrade() const { return mUpgrade; }

inline unsigned int ButtonTechUpgrade::GetLevel() const { return mLevel; }

inline bool ButtonTechUpgrade::IsUnlocked() const { return mUnlocked; }

inline void ButtonTechUpgrade::ClearButtonsToEnable() { mButtonsToEnable.clear(); }
inline void ButtonTechUpgrade::AddButtonToEnable(ButtonTechUpgrade * b)
{
    mButtonsToEnable.emplace_back(b);
}

inline void ButtonTechUpgrade::SetOnMouseOver(const std::function<void()> & f)
{
    mFuncOnMouseOver = f;
}

inline void ButtonTechUpgrade::SetOnMouseOut(const std::function<void()> & f)
{
    mFuncOnMouseOut = f;
}

} // namespace game
