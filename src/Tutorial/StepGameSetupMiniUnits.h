#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <vector>

namespace sgl
{
    namespace sgui { class Slider; }
}

namespace game
{

class GameHUD;
class FocusArea;

class StepGameSetupMiniUnits : public TutorialInfoStep
{
public:
    StepGameSetupMiniUnits(GameHUD * HUD, const std::vector<int> & values);
    ~StepGameSetupMiniUnits();

    void OnStart() override;

private:
    void HandleSlider(sgl::sgui::Slider * slider, int target);

private:
    FocusArea * mFocusArea = nullptr;
};

} // namespace game
