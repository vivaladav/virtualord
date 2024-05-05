#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class FocusArea;
class PanelClickFilter;
class PanelInfoTutorial;
class PanelObjectActions;

class StepGameUnitConquerCellsIcon : public TutorialStep
{
public:
    StepGameUnitConquerCellsIcon(PanelObjectActions * panel);
    ~StepGameUnitConquerCellsIcon();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;
};

} // namespace game
