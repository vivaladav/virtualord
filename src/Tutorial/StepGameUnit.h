#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class Game;
class Player;
class Unit;

class StepGameUnit : public TutorialInfoStep
{
public:
    StepGameUnit(const Game * game, const Unit * unit);
    ~StepGameUnit();

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;

    const Unit * mUnit = nullptr;
};

} // namespace game
