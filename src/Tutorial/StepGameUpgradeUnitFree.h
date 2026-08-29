#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <unordered_map>

namespace sgl
{
    namespace sgui { class AbstractButton; }
}

namespace game
{

class GameHUD;
class FocusArea;

class StepGameUpgradeUnitFree : public TutorialInfoStep
{
public:
    StepGameUpgradeUnitFree(GameHUD * HUD);
    ~StepGameUpgradeUnitFree();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;

    std::unordered_map<sgl::sgui::AbstractButton *, unsigned int> mCallbacks;

    GameHUD * mHUD = nullptr;
};

} // namespace game
