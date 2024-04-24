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
    }
}

namespace game
{

class ButtonPlanetMap;
class SecondaryButtonPlanetMap;
class Player;

enum TerritoryStatus : unsigned int;

class PanelPlanetActionExplore : public sgl::sgui::Widget
{
public:
    PanelPlanetActionExplore(Player * player, int money, int energy, int material);

    void ShowAction();
    void ShowResult(bool success);

    void AddOnButtonOkClickFunction(const std::function<void()> & f);
    void AddOnButtonCancelClickFunction(const std::function<void()> & f);

    sgl::sgui::AbstractButton * GetButtonOk() const;
    sgl::sgui::AbstractButton * GetButtonCancel() const;

private:
    void CreateContentStart(int money, int energy, int material);
    void CreateContentFailure();
    void CreateContentSuccess();

    void HandlePositionChanged() override;

    void UpdatePositions();

private:
    sgl::graphic::Image * mBg = nullptr;
    sgl::graphic::Text * mTitle = nullptr;

    sgl::sgui::Widget * mContentStart = nullptr;
    sgl::sgui::Widget * mContentFailure = nullptr;
    sgl::sgui::Widget * mContentSuccess = nullptr;

    sgl::sgui::Label * mLabelMoney = nullptr;
    sgl::sgui::Label * mLabelEnergy = nullptr;
    sgl::sgui::Label * mLabelMaterial = nullptr;

    ButtonPlanetMap * mButtonOk = nullptr;
    SecondaryButtonPlanetMap * mButtonCancel = nullptr;

    Player * mPlayer = nullptr;

    int mCostMoney = 0;
    int mCostenergy = 0;
    int mCostmaterial = 0;
};

} // namespace game
