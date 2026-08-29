#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <unordered_map>

namespace sgl
{
    namespace sgui
    {
        class AbstractButton;
        class Slider;
    }
}

namespace game
{

class DialogTrading;
class GameHUD;
class FocusArea;

class StepGameDialogTradingBuy : public TutorialInfoStep
{
public:
    StepGameDialogTradingBuy(GameHUD * HUD);
    ~StepGameDialogTradingBuy();

    void OnStart() override;

private:
    void HandleButton(unsigned int res, unsigned int value);

private:
    FocusArea * mFocusArea = nullptr;

    std::unordered_map<sgl::sgui::AbstractButton *, unsigned int> mCallbacks;

    const DialogTrading * mDialog = nullptr;
};

} // namespace game
