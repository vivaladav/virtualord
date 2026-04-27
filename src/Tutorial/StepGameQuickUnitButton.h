#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace sgl
{
    namespace sgui { class AbstractButton; }
}

namespace game
{

class FocusArea;
class GameHUD;

class StepGameQuickUnitButton : public TutorialInfoStep
{
public:
    StepGameQuickUnitButton(GameHUD * hud, int indButton);
    ~StepGameQuickUnitButton();

private:
    FocusArea * mFocusArea = nullptr;
    sgl::sgui::AbstractButton * mButton = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
