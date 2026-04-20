#pragma once

#include "Cell2D.h"
#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class IsoMap;

class StepGameConquerCellsSimple : public TutorialInfoStep
{
public:
    StepGameConquerCellsSimple(const IsoMap * isoMap, const Cell2D & cellActionStart,
                               const Cell2D & cellTarget);
    ~StepGameConquerCellsSimple();

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;

    const Cell2D & mCellActionStart;
    const Cell2D mCellTarget;
};

} // namespace game
