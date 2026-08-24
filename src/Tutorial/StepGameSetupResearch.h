#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <vector>
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

class GameHUD;
class FocusArea;

class StepGameSetupResearch : public TutorialInfoStep
{
public:
    StepGameSetupResearch(GameHUD * HUD, const std::vector<int> & values);
    ~StepGameSetupResearch();

    void OnStart() override;

private:
    void HandleSlider(sgl::sgui::Slider * slider, int target);

private:
    FocusArea * mFocusArea = nullptr;

    std::unordered_map<sgl::sgui::Slider *, unsigned int> mCallbacksSli;

    sgl::sgui::AbstractButton * mBtn = nullptr;
    unsigned int mCallbackBtn = 0;
};

} // namespace game
