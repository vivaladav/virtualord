#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <unordered_map>

namespace sgl { namespace sgui { class AbstractButton; } }

namespace game
{

class FocusArea;
class GameHUD;

class StepGameSecondaryMissionGoal : public TutorialInfoStep
{
public:
    StepGameSecondaryMissionGoal(GameHUD * HUD, int goal);
    ~StepGameSecondaryMissionGoal();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;

    std::unordered_map<sgl::sgui::AbstractButton *, unsigned int> mCallbacks;

    GameHUD * mHUD = nullptr;
};

} // namespace game
