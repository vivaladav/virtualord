#pragma once

#include "Cell2D.h"
#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class FocusArea;
class Game;
class IsoMap;

class StepGameConquerCellsSimple : public TutorialInfoStep
{
public:
    StepGameConquerCellsSimple(const Game * game, const IsoMap * isoMap, const Cell2D & start,
                               const Cell2D & target, const sgl::core::Pointd2D & p0);
    ~StepGameConquerCellsSimple();

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;

    const Cell2D & mCellActionStart;
    const Cell2D mCellTarget;
};

} // namespace game
