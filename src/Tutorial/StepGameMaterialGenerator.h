#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class FocusArea;
class GameObject;

class StepGameMaterialGenerator : public TutorialInfoStep
{
public:
    StepGameMaterialGenerator(const GameObject * materialGen, const sgl::core::Pointd2D & p0);
    ~StepGameMaterialGenerator();

private:
    FocusArea * mFocusArea = nullptr;
};

} // namespace game
