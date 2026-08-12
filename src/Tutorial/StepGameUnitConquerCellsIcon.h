#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class Game;
class PanelObjectActions;
class ScreenGame;

class StepGameUnitConquerCellsIcon : public TutorialInfoStep
{
public:
    StepGameUnitConquerCellsIcon(const Game * game, ScreenGame * screen, PanelObjectActions * panel);
    ~StepGameUnitConquerCellsIcon();

    void OnStart() override;
    void OnEnd() override;

private:
    FocusArea * mFocusArea = nullptr;
    ScreenGame * mScreen = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;

    bool mSelAllowed;
};

} // namespace game
