#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class Base;
class Game;
class IsoFocusArea;
class IsoMap;

class StepGameSelectBase : public TutorialInfoStep
{
public:
    StepGameSelectBase(const Game * game, const IsoMap * im, const Base * b);
    ~StepGameSelectBase();

    void Update(float delta) override;

private:
    IsoFocusArea * mFocusArea = nullptr;

    const Base * mBase = nullptr;
};

} // namespace game
