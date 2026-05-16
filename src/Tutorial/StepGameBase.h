#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class Base;
class FocusArea;
class Game;

class StepGameBase : public TutorialInfoStep
{
public:
    StepGameBase(const Game * game, const Base * b);
    ~StepGameBase();

    void OnStart() override;

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;

    const Base * mBase = nullptr;

    bool mCheckBaseSelected = false;
};

} // namespace game
