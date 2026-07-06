#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class FocusArea;
class GameObject;

class StepGameConquerEnergyGenIntro : public TutorialInfoStep
{
public:
    StepGameConquerEnergyGenIntro(const GameObject * gen, const sgl::core::Pointd2D & p0);
    ~StepGameConquerEnergyGenIntro();

private:
    FocusArea * mFocusArea = nullptr;
};

} // namespace game
