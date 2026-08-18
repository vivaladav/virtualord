#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class FocusArea;
class Game;
class PanelObjectActions;

class StepGameSpawnIcon : public TutorialInfoStep
{
public:
    StepGameSpawnIcon(const Game * game, Screen * screen, PanelObjectActions * panel,
                         const sgl::core::Pointd2D & p0);
    ~StepGameSpawnIcon();

private:
    FocusArea * mFocusArea = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
