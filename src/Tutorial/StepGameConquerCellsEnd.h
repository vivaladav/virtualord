#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class IsoMap;
class Player;
class Unit;

class StepGameConquerCellsEnd : public TutorialInfoStep
{
public:
    StepGameConquerCellsEnd(const IsoMap * isoMap, const Player * p);
    ~StepGameConquerCellsEnd();

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;

    Unit * mUnit = nullptr;
};

} // namespace game
