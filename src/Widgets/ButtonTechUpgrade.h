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

    void SetUpgrade(TechUpgradeId upgrade);

    unsigned int GetLevel() const;
    void SetLevel(unsigned int lvl);

    void SetUnlocked(bool unlocked);

    void ClearLinks();
    void AddLink(sgl::sgui::Image * link);

private:
    void HandleMouseOver() override;

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

    sgl::graphic::Image * mBg = nullptr;
    sgl::graphic::Image * mIcon = nullptr;

    sgl::graphic::Image * mBgLevel = nullptr;
    sgl::graphic::Text * mLabelLevel = nullptr;

    unsigned int mLevel = 0;
    bool mLevelVisible = false;
    bool mIconVisible = false;
    bool mUnlocked = false;
};

inline unsigned int ButtonTechUpgrade::GetLevel() const { return mLevel; }

} // namespace game
