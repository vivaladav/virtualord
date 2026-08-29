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

    std::unordered_map<sgl::sgui::AbstractButton *, unsigned int> mCallbacks;
};

} // namespace game
