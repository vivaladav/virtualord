#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class GameObject;

class StepGameSetObjectFatalHit : public TutorialStep
{
public:
    StepGameSetObjectFatalHit(GameObject * obj, bool enabled);

    void OnStart() override;

private:
    GameObject * mObj = nullptr;

    bool mEnabled;
};

} // namespace game
