#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class Game;
class IsoMap;

struct Cell2D;

class StepGameConquerCells : public TutorialInfoStep
{
public:
    StepGameConquerCells(const Game * game, const IsoMap * isoMap, const Cell2D & cellActionStart);
    ~StepGameConquerCells();

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;

    const Cell2D & mCellActionStart;
};

} // namespace game
