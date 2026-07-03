#pragma once

#include "Tutorial/TutorialStep.h"

#include <vector>

namespace game
{

class GameObject;

enum AttackMode : unsigned int;

class StepGameSetObjectAttackMode : public TutorialStep
{
public:
    StepGameSetObjectAttackMode(GameObject * obj, AttackMode mode);
    StepGameSetObjectAttackMode(const std::vector<GameObject *> && objs, AttackMode mode);

    void OnStart() override;

private:
    std::vector<GameObject *> mObjs;
    AttackMode mMode;
};

} // namespace game
