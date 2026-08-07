#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class FocusArea;
class GameObject;

class StepGameConquerMaterialGenIntro : public TutorialInfoStep
{
public:
    StepGameConquerMaterialGenIntro(const GameObject * materialGen,
                                    const sgl::core::Pointd2D & p0 = sgl::core::Pointd2D(1150, 450));
    ~StepGameConquerMaterialGenIntro();

private:
    FocusArea * mFocusArea = nullptr;
};

} // namespace game
