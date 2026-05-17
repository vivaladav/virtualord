#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class Base;
class FocusArea;
class Game;

class StepGameSelectBase : public TutorialInfoStep
{
public:
    StepGameSelectBase(const Game * game, const Base * b);
    ~StepGameSelectBase();

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;

    const Base * mBase = nullptr;
};

} // namespace game
