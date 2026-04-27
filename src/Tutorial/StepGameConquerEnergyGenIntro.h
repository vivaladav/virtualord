#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class GameObject;

class StepGameConquerEnergyGenIntro : public TutorialInfoStep
{
public:
    StepGameConquerEnergyGenIntro(const GameObject * gen);
    ~StepGameConquerEnergyGenIntro();

private:
    FocusArea * mFocusArea = nullptr;
};

} // namespace game
