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
class SecondaryButtonPlanetMap;
class Player;

enum TerritoryStatus : unsigned int;

class PanelPlanetActionConquer : public sgl::sgui::Widget
{
public:
    PanelPlanetActionConquer();

    void AddOnButtonOkClickFunction(const std::function<void()> & f);
    void AddOnButtonCancelClickFunction(const std::function<void()> & f);

    sgl::sgui::AbstractButton * GetButtonOk() const;
    sgl::sgui::AbstractButton * GetButtonCancel() const;

private:
    void CreateContentStart();

    void HandlePositionChanged() override;

    void UpdatePositions();

private:
    sgl::graphic::Image * mBg = nullptr;
    sgl::graphic::Text * mTitle = nullptr;

    sgl::sgui::Widget * mContentStart = nullptr;

    ButtonPlanetMap * mButtonOk = nullptr;
    SecondaryButtonPlanetMap * mButtonCancel = nullptr;
};

} // namespace game
