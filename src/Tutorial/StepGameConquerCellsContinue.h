#pragma once

#include "Cell2D.h"
#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class Game;
class IsoFocusArea;
class IsoMap;
class OverlayCellConquest;

class StepGameConquerCellsContinue : public TutorialInfoStep
{
public:
    StepGameConquerCellsContinue(const Game * game, const IsoMap * isoMap,  const Cell2D & target,
                                 const OverlayCellConquest * overlay, const sgl::core::Pointd2D & p0);
    ~StepGameConquerCellsContinue();

    void Update(float delta) override;

private:
    IsoFocusArea * mFocusArea = nullptr;

    const OverlayCellConquest * mOverlay = nullptr;
    const unsigned int mTarget;
};

} // namespace game
