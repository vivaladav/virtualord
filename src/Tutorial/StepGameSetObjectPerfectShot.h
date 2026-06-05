#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class GameObject;

class StepGameSetObjectPerfectShot : public TutorialStep
{
public:
    StepGameSetObjectPerfectShot(GameObject * obj, bool enabled);

    void OnStart() override;

private:
    GameObject * mObj = nullptr;

    bool mEnabled;
};

} // namespace game
