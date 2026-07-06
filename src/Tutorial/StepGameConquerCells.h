#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class Game;
class IsoFocusArea;
class IsoMap;

struct Cell2D;

class StepGameConquerCells : public TutorialInfoStep
{
public:
    StepGameConquerCells(const Game * game, const IsoMap * isoMap, const Cell2D & cellActionStart);
    ~StepGameConquerCells();

    void Update(float delta) override;

private:
    IsoFocusArea * mFocusArea = nullptr;

    const Cell2D & mCellActionStart;
};

} // namespace game
