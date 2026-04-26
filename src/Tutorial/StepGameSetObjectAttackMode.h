#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class GameObject;

enum AttackMode : unsigned int;

class StepGameSetObjectAttackMode : public TutorialStep
{
public:
    StepGameSetObjectAttackMode(GameObject * obj, AttackMode mode);

    void OnStart() override;

private:
    GameObject * mObj = nullptr;
    AttackMode mMode;
};

} // namespace game
