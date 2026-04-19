#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class GameObject;

class StepGameMaterialGenerator : public TutorialInfoStep
{
public:
    StepGameMaterialGenerator(const GameObject * materialGen);
    ~StepGameMaterialGenerator();

private:
    FocusArea * mFocusArea = nullptr;
};

} // namespace game
