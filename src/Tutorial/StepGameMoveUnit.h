#pragma once

#include "Game.h"
#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class Game;
class IsoFocusArea;
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
    IsoFocusArea * mFocusArea = nullptr;

    Unit * mUnit = nullptr;
};

} // namespace game
