#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class StepGameSingleInfo : public TutorialInfoStep
{
public:
    StepGameSingleInfo(const sgl::core::Pointd2D & p0, const char * text);
};

} // namespace game
