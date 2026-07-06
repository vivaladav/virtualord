#pragma once

#include "Tutorial/TutorialStep.h"

#include <vector>

namespace game
{

class GameObject;

class StepGameSetObjectFatalHit : public TutorialStep
{
public:
    StepGameSetObjectFatalHit(GameObject * obj, bool enabled);
    StepGameSetObjectFatalHit(const std::vector<GameObject *> && objs, bool enabled);

    void OnStart() override;

private:
    std::vector<GameObject *> mObjs;
    bool mEnabled;
};

} // namespace game
