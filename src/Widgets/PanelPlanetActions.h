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

    namespace sgui { class AbstractButton; }
}

namespace game
{

class ButtonPlanetMap;

enum TerritoryStatus : unsigned int;

class PanelPlanetActions : public sgl::sgui::Widget
{
public:
    enum Button : unsigned int
    {
        EXPLORE,
        CONQUER,
        SEND_AI,

        NUM_BUTTONS
    };

public:
    PanelPlanetActions();

    void AddOnButtonClickFunction(Button btn, const std::function<void()> & f);

    void UpdateButtons(TerritoryStatus status, bool isPlayerOccupier);

    sgl::sgui::AbstractButton * GetButton(Button btn) const;

private:
    void HandlePositionChanged() override;

    void HandleStateEnabled() override;
    void HandleStateDisabled() override;

    void UpdatePositions();

private:
    sgl::graphic::Image * mBg = nullptr;
    sgl::graphic::Text * mTitle = nullptr;

    ButtonPlanetMap * mButtons[NUM_BUTTONS];
};

} // namespace game
