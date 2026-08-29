#pragma once

#include "Cell2D.h"
#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class IsoFocusArea;
class IsoMap;

class StepGameCityIntro : public TutorialInfoStep
{
public:
    StepGameCityIntro(const Cell2D & tl, const Cell2D & br, const IsoMap * isoMap,
                      const sgl::core::Pointd2D & p0);
    ~StepGameCityIntro();

private:
    IsoFocusArea * mFocusArea = nullptr;
};

} // namespace game
