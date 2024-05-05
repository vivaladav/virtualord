#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class FocusArea;
class IsoMap;
class PanelClickFilter;
class PanelInfoTutorial;
class Player;
class Unit;

class StepGameMoveUnit : public TutorialStep
{
public:
    StepGameMoveUnit(const Player * p, const IsoMap * isoMap);
    ~StepGameMoveUnit();

    void OnStart() override;

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;

    Unit * mUnit = nullptr;
};

} // namespace game
