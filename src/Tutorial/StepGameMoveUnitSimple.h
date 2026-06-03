#pragma once

#include "Cell2D.h"
#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class IsoFocusArea;
class IsoMap;
class Unit;

class StepGameMoveUnitSimple : public TutorialInfoStep
{
public:
    StepGameMoveUnitSimple(const Unit * unit, const IsoMap * isoMap,
                             const Cell2D & target, const sgl::core::Pointd2D & p0);
    ~StepGameMoveUnitSimple();

    void Update(float delta) override;

private:
    IsoFocusArea * mFocusArea = nullptr;

    const Unit * mUnit = nullptr;

    const Cell2D mTarget;
};

} // namespace game
