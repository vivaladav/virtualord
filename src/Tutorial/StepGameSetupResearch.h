#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace sgl
{
    namespace sgui { class Slider; }
}

namespace game
{

class GameHUD;
class FocusArea;

class StepGameSetupResearch : public TutorialInfoStep
{
public:
    StepGameSetupResearch(GameHUD * HUD);
    ~StepGameSetupResearch();

    void OnStart() override;

private:
    void HandleSlider(sgl::sgui::Slider * slider, int target);

private:
    FocusArea * mFocusArea = nullptr;
};

} // namespace game
