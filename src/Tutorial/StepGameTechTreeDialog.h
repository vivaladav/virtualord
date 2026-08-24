#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <unordered_map>

namespace sgl
{
    namespace sgui { class AbstractButton; }
}

namespace game
{

class FocusArea;
class GameHUD;

enum TechUpgradeId : unsigned int;

class StepGameTechTreeDialog : public TutorialInfoStep
{
public:
    StepGameTechTreeDialog(GameHUD * HUD, TechUpgradeId upgradeID, bool showIntro);
    ~StepGameTechTreeDialog();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;

    std::unordered_map<sgl::sgui::AbstractButton *, unsigned int> mCallbacks;

    sgl::sgui::AbstractButton * mBtn = nullptr;
    unsigned int mCallbackBtn = 0;

    GameHUD * mHUD = nullptr;
};

} // namespace game
