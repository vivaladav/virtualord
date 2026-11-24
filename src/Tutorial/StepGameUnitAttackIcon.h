#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class FocusArea;
class Game;
class PanelObjectActions;

class StepGameUnitAttackIcon : public TutorialInfoStep
{
public:
    StepGameUnitAttackIcon(const Game * game, Screen * screen, PanelObjectActions * panel,
                           const char * text, const sgl::core::Pointd2D & p0);
    ~StepGameUnitAttackIcon();

    void SetPause(bool paused) override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
