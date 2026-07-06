#pragma once

#include "Cell2D.h"
#include "Player.h"
#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class IsoFocusArea;
class IsoMap;
class Player;
class Unit;

class StepGameBuildTowerEnd : public TutorialInfoStep
{
public:
    StepGameBuildTowerEnd(const IsoMap * isoMap, const Unit * unit, const Cell2D & cell,
                          const sgl::core::Pointd2D & p0);
    ~StepGameBuildTowerEnd();

    void Update(float delta) override;

private:
    IsoFocusArea * mFocusArea = nullptr;

    const Unit * mUnit = nullptr;

    const Cell2D mTarget;

    bool mBuildStarted = false;
};

} // namespace game
