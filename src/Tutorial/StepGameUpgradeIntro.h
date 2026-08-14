#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class FocusArea;
class Game;
class PanelObjectActions;

class StepGameUpgradeIntro : public TutorialInfoStep
{
public:
    StepGameUpgradeIntro(const Game * game, Screen * screen, PanelObjectActions * panel,
                         const char * text1, const sgl::core::Pointd2D & p0);
    ~StepGameUpgradeIntro();

private:
    FocusArea * mFocusArea = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
