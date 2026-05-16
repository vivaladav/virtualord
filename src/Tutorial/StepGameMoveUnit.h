#pragma once

#include "Game.h"
#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class Game;
class IsoMap;
class Player;
class Unit;

class StepGameMoveUnit : public TutorialInfoStep
{
public:
    StepGameMoveUnit(const Game * game, const Player * p, const IsoMap * isoMap);
    ~StepGameMoveUnit();

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;

    Unit * mUnit = nullptr;
};

} // namespace game
