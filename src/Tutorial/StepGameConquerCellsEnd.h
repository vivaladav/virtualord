#pragma once

#include "Cell2D.h"
#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class FocusArea;
class IsoMap;
class Player;
class Unit;

class StepGameConquerCellsEnd : public TutorialInfoStep
{
public:
    StepGameConquerCellsEnd(const IsoMap * isoMap, const Player * p, const Cell2D & cellEnd,
                            const sgl::core::Pointd2D & p0);
    ~StepGameConquerCellsEnd();

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;

    Unit * mUnit = nullptr;

    Cell2D mCellEnd;
};

} // namespace game
