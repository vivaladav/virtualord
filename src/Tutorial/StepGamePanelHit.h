#pragma once

#include "Cell2D.h"
#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class IsoFocusArea;
class IsoMap;
class Unit;

class StepGamePanelHit : public TutorialInfoStep
{
public:
    StepGamePanelHit(const IsoMap * isoMap, const Cell2D & targetBR, const Cell2D & targetTL,
                     const sgl::core::Pointd2D & p0);
    ~StepGamePanelHit();

    void Update(float delta) override;

private:
    IsoFocusArea * mFocusArea = nullptr;

    const Unit * mUnit = nullptr;

    const Cell2D mTargetBR;
    const Cell2D mTargetTL;
};

} // namespace game
