#pragma once

#include "Cell2D.h"
#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class IsoMap;
class Unit;

class StepGameWallBuildEnd : public TutorialInfoStep
{
public:
    StepGameWallBuildEnd(const IsoMap * isoMap, const Unit * u, const Cell2D & cellEnd);
    ~StepGameWallBuildEnd();

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;

    const Unit * mUnit = nullptr;

    Cell2D mCellEnd;

    bool mBuildStarted = false;
};

} // namespace game
