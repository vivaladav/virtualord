#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class StepGameConnectStructIntro : public TutorialInfoStep
{
public:
    StepGameConnectStructIntro(const sgl::core::Pointd2D & p0, const char * text = "TUT_GAME_CONNECT_STRUCT");
};

} // namespace game
