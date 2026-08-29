#pragma once

#include "Cell2D.h"
#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class IsoFocusArea;
class IsoMap;
class Player;
class MiniUnitsGroup;

class StepGameSetTarget : public TutorialInfoStep
{
public:
    StepGameSetTarget(const IsoMap * isoMap, const MiniUnitsGroup * group, const Cell2D & cell,
                      const sgl::core::Pointd2D & p0);
    ~StepGameSetTarget();

    void Update(float delta) override;

private:
    IsoFocusArea * mFocusArea = nullptr;

    const MiniUnitsGroup * mGroup = nullptr;

    const Cell2D mTarget;

    bool mBuildStarted = false;
};

} // namespace game
