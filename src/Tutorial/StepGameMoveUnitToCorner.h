#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class IsoMap;
class Player;
class Unit;

class StepGameMoveUnitToCorner : public TutorialInfoStep
{
public:
    StepGameMoveUnitToCorner(const Player * p, const IsoMap * isoMap);
    ~StepGameMoveUnitToCorner();

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;

    Unit * mUnit = nullptr;
};

} // namespace game
