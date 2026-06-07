#pragma once

#include "Cell2D.h"
#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class IsoFocusArea;
class IsoMap;
class Unit;

class StepGameMoveUnitToArea : public TutorialInfoStep
{
public:
    StepGameMoveUnitToArea(const Unit * unit, const IsoMap * isoMap,
                           const Cell2D & areaTL, const Cell2D & areaBR, const Cell2D & target,
                           const char * intro, const sgl::core::Pointd2D & p0);
    ~StepGameMoveUnitToArea();

    void Update(float delta) override;

private:
    IsoFocusArea * mFocusArea = nullptr;

    const Unit * mUnit = nullptr;

    const Cell2D mTarget;
};

} // namespace game
