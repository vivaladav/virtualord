#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class GameObject;;

class StepGameWaitObjectIdle : public TutorialStep
{
public:
    StepGameWaitObjectIdle(const GameObject * obj);

    void Update(float delta) override;

private:
    const GameObject * mObj = nullptr;
};

} // namespace game
