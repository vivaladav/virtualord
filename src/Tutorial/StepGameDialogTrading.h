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

class GameHUD;
class FocusArea;

class StepGameDialogTrading : public TutorialInfoStep
{
public:
    StepGameDialogTrading(GameHUD * HUD);
    ~StepGameDialogTrading();

    void OnStart() override;

private:
    void HandleButton(sgl::sgui::AbstractButton * btn);

private:
    FocusArea * mFocusArea = nullptr;
};

} // namespace game
