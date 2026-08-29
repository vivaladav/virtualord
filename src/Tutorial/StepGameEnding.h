#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class StepGameEnding : public TutorialInfoStep
{
public:
    StepGameEnding(const sgl::core::Pointd2D & p0);
};

} // namespace game
