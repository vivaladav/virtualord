#pragma once

#include "Tutorial/TutorialStep.h"

#include <vector>

namespace game
{

class GameObject;

class StepGameSetObjectHealth : public TutorialStep
{
public:
    StepGameSetObjectHealth(GameObject * obj, float val);
    StepGameSetObjectHealth(const std::vector<GameObject *> && objs, float val);

    void OnStart() override;

private:
    std::vector<GameObject *> mObjs;
    float mVal = 0.f;
};

} // namespace game
