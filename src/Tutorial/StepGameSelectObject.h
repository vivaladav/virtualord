#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class Game;
class GameObject;
class IsoFocusArea;
class IsoMap;

class StepGameSelectObject : public TutorialInfoStep
{
public:
    StepGameSelectObject(const Game * game, const IsoMap * im, const GameObject * obj,
                         const char * text, const sgl::core::Pointd2D & p0);
    ~StepGameSelectObject();

    void Update(float delta) override;

private:
    IsoFocusArea * mFocusArea = nullptr;

    const GameObject * mObj = nullptr;
};

} // namespace game
