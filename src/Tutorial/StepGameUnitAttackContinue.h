#pragma once

#include "Cell2D.h"
#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class Game;
class IsoFocusArea;
class IsoMap;
class Unit;

class StepGameUnitAttackContinue : public TutorialInfoStep
{
public:
    StepGameUnitAttackContinue(const Game * game, const Unit * unit, const IsoMap * isoMap,
                             const Cell2D & target, const sgl::core::Pointd2D & p0);
    ~StepGameUnitAttackContinue();

    void Update(float delta) override;

private:
    IsoFocusArea * mFocusArea = nullptr;

    const Unit * mUnit = nullptr;

    const Cell2D mTarget;
};

} // namespace game
