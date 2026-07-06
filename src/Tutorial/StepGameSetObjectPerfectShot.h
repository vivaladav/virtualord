#pragma once

#include "Tutorial/TutorialStep.h"

#include <vector>

namespace game
{

class GameObject;

class StepGameSetObjectPerfectShot : public TutorialStep
{
public:
    StepGameSetObjectPerfectShot(GameObject * obj, bool enabled);
    StepGameSetObjectPerfectShot(const std::vector<GameObject *> && objs, bool enabled);

    void OnStart() override;

private:
    std::vector<GameObject *> mObjs;
    bool mEnabled;
};

} // namespace game
