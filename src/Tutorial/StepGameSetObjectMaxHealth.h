#pragma once

#include "Tutorial/TutorialStep.h"

#include <vector>

namespace game
{

class GameObject;

class StepGameSetObjectMaxHealth : public TutorialStep
{
public:
    StepGameSetObjectMaxHealth(GameObject * obj, float val);
    StepGameSetObjectMaxHealth(const std::vector<GameObject *> && objs, float val);

    void OnStart() override;

private:
    std::vector<GameObject *> mObjs;
    float mVal = 0.f;
};

} // namespace game
