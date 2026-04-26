#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class GameObject;

class StepGameSetObjectMaxHealth : public TutorialStep
{
public:
    StepGameSetObjectMaxHealth(GameObject * obj, float val);

    void OnStart() override;

private:
    GameObject * mObj = nullptr;
    float mVal = 0.f;
};

} // namespace game
