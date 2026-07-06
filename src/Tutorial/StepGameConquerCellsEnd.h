#pragma once

#include "Cell2D.h"
#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class Game;
class IsoFocusArea;
class IsoMap;
class Unit;

class StepGameConquerCellsEnd : public TutorialInfoStep
{
public:
    StepGameConquerCellsEnd(const Game * game, const IsoMap * isoMap, const Unit * unit,
                            const Cell2D & cellEnd, const sgl::core::Pointd2D & p0);
    ~StepGameConquerCellsEnd();

    void Update(float delta) override;

private:
    IsoFocusArea * mFocusArea = nullptr;

    const Unit * mUnit = nullptr;

    Cell2D mCellEnd;
};

} // namespace game
