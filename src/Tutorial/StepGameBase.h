#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class Base;
class FocusArea;
class Game;
class IsoFocusArea;
class IsoMap;

class StepGameBase : public TutorialInfoStep
{
public:
    StepGameBase(const Game * game, const IsoMap * im, const Base * b);
    ~StepGameBase();

    void OnStart() override;

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;
    IsoFocusArea * mIsoFocusArea = nullptr;

    const Base * mBase = nullptr;

    bool mCheckBaseSelected = false;
};

} // namespace game
