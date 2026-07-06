#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class StepGameEnemyIntro : public TutorialInfoStep
{
public:
    StepGameEnemyIntro(const sgl::core::Pointd2D & p0);
};

} // namespace game
