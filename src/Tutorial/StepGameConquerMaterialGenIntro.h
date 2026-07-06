#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class GameObject;

class StepGameConquerMaterialGenIntro : public TutorialInfoStep
{
public:
    StepGameConquerMaterialGenIntro(const GameObject * materialGen);
    ~StepGameConquerMaterialGenIntro();

private:
    FocusArea * mFocusArea = nullptr;
};

} // namespace game
