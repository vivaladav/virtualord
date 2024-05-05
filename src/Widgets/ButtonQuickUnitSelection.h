#pragma once

#include <sgl/sgui/ImageButton.h>

namespace sgl
{
    namespace graphic
    {
        class Image;
        class Text;
    }
}

namespace game
{

class DigitsDisplay;
class ScreenGame;
class Unit;

class ButtonQuickUnitSelection : public sgl::sgui::ImageButton
{
public:
    ButtonQuickUnitSelection(int index, ScreenGame * sg, sgl::sgui::Widget * parent = nullptr);

    void ClearUnit();
    Unit * GetUnit() const;
    void SetUnit(Unit * unit);

private:
    void OnStateChanged(sgl::sgui::AbstractButton::VisualState state) override;

    void HandlePositionChanged() override;

    void HandleKeyDown(sgl::core::KeyboardEvent & event) override;
    void HandleKeyUp(sgl::core::KeyboardEvent & event) override;

    void HandleButtonUp() override;

    void UpdateValues();

private:
    Unit * mUnit = nullptr;

    ScreenGame * mScreenGame = nullptr;

    sgl::graphic::Text * mShortcut = nullptr;
    sgl::graphic::Image * mImgUnit = nullptr;
    sgl::graphic::Image * mIconEnergy = nullptr;
    DigitsDisplay * mDisplayEnergy = nullptr;
    sgl::graphic::Image * mIconHealth = nullptr;
    DigitsDisplay * mDisplayHealth = nullptr;

    int mShortcutKey = -1;

    unsigned int mFunValChangedId = 0;
};

inline Unit * ButtonQuickUnitSelection::GetUnit() const { return mUnit; }

} // namespace game
