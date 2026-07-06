#pragma once

#include "Cell2D.h"
#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class Game;

class IsoMap;
class IsoFocusArea;
class StepGameWallBuildStart : public TutorialInfoStep
{
public:
    StepGameWallBuildStart(const Game * game, const IsoMap * isoMap,
                           const Cell2D & cellActionStart, const Cell2D & target);
    ~StepGameWallBuildStart();

    void Update(float delta) override;

private:
    IsoFocusArea * mFocusArea = nullptr;

    const Cell2D & mCellActionStart;
    const Cell2D mTarget;
};

} // namespace game
