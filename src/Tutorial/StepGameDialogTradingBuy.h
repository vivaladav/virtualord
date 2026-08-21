#pragma once

#include "Tutorial/TutorialInfoStep.h"

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

    const DialogTrading * mDialog = nullptr;
};

} // namespace game
