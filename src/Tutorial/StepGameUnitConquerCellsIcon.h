#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class Game;
class PanelObjectActions;

class StepGameUnitConquerCellsIcon : public TutorialInfoStep
{
public:
    StepGameUnitConquerCellsIcon(const Game * game, Screen * screen, PanelObjectActions * panel);
    ~StepGameUnitConquerCellsIcon();

private:
    FocusArea * mFocusArea = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
