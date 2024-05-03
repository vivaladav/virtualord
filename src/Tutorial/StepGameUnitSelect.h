#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class FocusArea;
class PanelClickFilter;
class PanelInfoTutorial;
class Player;
class Unit;

class StepGameUnitSelect : public TutorialStep
{
public:
    StepGameUnitSelect(const Player * p);
    ~StepGameUnitSelect();

    void OnStart() override;

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;

    Unit * mUnit = nullptr;
};

} // namespace game
