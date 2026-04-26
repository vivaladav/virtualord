#pragma once

#include "Cell2D.h"
#include "Player.h"
#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class IsoMap;
class Player;
class Unit;

class StepGameBuildTowerEnd : public TutorialInfoStep
{
public:
    StepGameBuildTowerEnd(const IsoMap * isoMap, const Unit * unit, const Cell2D & cell);
    ~StepGameBuildTowerEnd();

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;

    const Unit * mUnit = nullptr;

    const Cell2D mTarget;

    bool mBuildStarted = false;
};

} // namespace game
