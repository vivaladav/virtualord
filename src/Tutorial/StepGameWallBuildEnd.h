#pragma once

#include "Cell2D.h"
#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class IsoMap;
class Player;
class Unit;

class StepGameWallBuildEnd : public TutorialInfoStep
{
public:
    StepGameWallBuildEnd(const IsoMap * isoMap, const Player * p, const Cell2D & cellEnd);
    ~StepGameWallBuildEnd();

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;

    Unit * mUnit = nullptr;

    Cell2D mCellEnd;

    bool mBuildStarted = false;
};

} // namespace game
