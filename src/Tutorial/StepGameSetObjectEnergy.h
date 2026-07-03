#pragma once

#include "Tutorial/TutorialStep.h"

#include <vector>

namespace game
{

class GameObject;

class StepGameSetObjectEnergy : public TutorialStep
{
public:
    StepGameSetObjectEnergy(GameObject * obj, float val);
    StepGameSetObjectEnergy(const std::vector<GameObject *> && obj, float val);

    void OnStart() override;

private:
    std::vector<GameObject *> mObjs;
    float mVal = 0.f;
};

} // namespace game
