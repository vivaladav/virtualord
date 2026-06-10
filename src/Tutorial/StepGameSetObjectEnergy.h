#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class GameObject;

class StepGameSetObjectEnergy : public TutorialStep
{
public:
    StepGameSetObjectEnergy(GameObject * obj, float val);

    void OnStart() override;

private:
    GameObject * mObj = nullptr;
    float mVal = 0.f;
};

} // namespace game
