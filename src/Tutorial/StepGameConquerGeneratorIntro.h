#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class FocusArea;
class GameObject;

class StepGameConquerGeneratorIntro : public TutorialInfoStep
{
public:
    StepGameConquerGeneratorIntro(const GameObject * gen, const char * text,
                                  const sgl::core::Pointd2D & p0);
    ~StepGameConquerGeneratorIntro();

private:
    FocusArea * mFocusArea = nullptr;
};

} // namespace game
