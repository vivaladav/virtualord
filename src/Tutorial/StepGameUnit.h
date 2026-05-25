#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class Game;
class IsoFocusArea;
class IsoMap;
class Player;
class Unit;

class StepGameUnit : public TutorialInfoStep
{
public:
    StepGameUnit(const Game * game, const IsoMap * im, const Unit * unit);
    ~StepGameUnit();

    void Update(float delta) override;

private:
    IsoFocusArea * mFocusArea = nullptr;

    const Unit * mUnit = nullptr;
};

} // namespace game
