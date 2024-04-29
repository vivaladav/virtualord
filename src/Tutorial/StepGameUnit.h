#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class FocusArea;
class PanelClickFilter;
class PanelInfoTutorial;
class Player;
class Unit;

class StepGameUnit : public TutorialStep
{
public:
    StepGameUnit(const Player * p);
    ~StepGameUnit();

    void OnStart() override;

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;

    Unit * mUnit = nullptr;
    bool mCheckUnitSelected = false;
};

} // namespace game
