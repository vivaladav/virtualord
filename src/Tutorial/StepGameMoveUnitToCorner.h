#pragma once

#include "Cell2D.h"
#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class FocusArea;
class IsoMap;
class Player;
class Unit;

class StepGameMoveUnitToCorner : public TutorialInfoStep
{
public:
    StepGameMoveUnitToCorner(const Player * p, const IsoMap * isoMap,
                             const Cell2D & target, const sgl::core::Pointd2D & p0);
    ~StepGameMoveUnitToCorner();

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;

    Unit * mUnit = nullptr;

    const Cell2D mTarget;
};

} // namespace game
