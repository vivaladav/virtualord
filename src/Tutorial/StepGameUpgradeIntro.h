#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class PanelObjectActions;

class StepGameUpgradeIntro : public TutorialInfoStep
{
public:
    StepGameUpgradeIntro(PanelObjectActions * panel);
    ~StepGameUpgradeIntro();

private:
    FocusArea * mFocusArea = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
