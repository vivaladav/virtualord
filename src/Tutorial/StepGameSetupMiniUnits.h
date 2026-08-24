#pragma once

#include "GameObjects/GameObjectTypes.h"
#include "Tutorial/TutorialInfoStep.h"

#include <unordered_map>
#include <vector>

namespace sgl
{
    namespace sgui
    {
        class AbstractButton;
        class AbstractSlider;
    }
}

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
    StepGameSetupMiniUnits(GameHUD * HUD, const std::vector<int> & values, GameObjectTypeId type);
    ~StepGameSetupMiniUnits();

    void OnStart() override;

private:
    void HandleSlider(sgl::sgui::Slider * slider, int target);

private:
    FocusArea * mFocusArea = nullptr;

    std::unordered_map<sgl::sgui::AbstractButton *, unsigned int> mCallbacksBtn;
    std::unordered_map<sgl::sgui::AbstractSlider *, unsigned int> mCallbacksSli;
};

} // namespace game
