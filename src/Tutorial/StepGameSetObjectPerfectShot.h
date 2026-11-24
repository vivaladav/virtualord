#pragma once

#include "Tutorial/TutorialStep.h"

#include <vector>

namespace game
{

class GameObject;

class StepGameSetObjectPerfectShot : public TutorialStep
{
public:
    StepGameSetObjectPerfectShot(GameObject * obj, bool enabled, bool forceFatal);
    StepGameSetObjectPerfectShot(const std::vector<GameObject *> && objs, bool enabled,
                                 bool forceFatal);

    void OnStart() override;

private:
    std::vector<GameObject *> mObjs;
    bool mEnabled;
    bool mForceFatal;
};

} // namespace game
