#pragma once

#include "Cell2D.h"
#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class CityGroup;
class Game;
class IsoFocusArea;
class IsoMap;

class StepGameWaitCityConquered : public TutorialInfoStep
{
public:
    StepGameWaitCityConquered(CityGroup * city, const Cell2D & tl, const Cell2D & br,
                              const IsoMap * isoMap, const Game * game,
                              const sgl::core::Pointd2D & p0);
    ~StepGameWaitCityConquered();

    void Update(float delta) override;

private:
    IsoFocusArea * mFocusArea = nullptr;

    CityGroup * mCity = nullptr;
};

} // namespace game
