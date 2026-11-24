#pragma once

#include "Cell2D.h"
#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class Game;
class IsoFocusArea;
class IsoMap;
class GameObject;

class StepGameObjectAttackSimple : public TutorialInfoStep
{
public:
    StepGameObjectAttackSimple(const Game * game, const GameObject * Object,
                               const IsoMap * isoMap, const Cell2D & target,
                               const sgl::core::Pointd2D & p0);
    ~StepGameObjectAttackSimple();

    void Update(float delta) override;

private:
    IsoFocusArea * mFocusArea = nullptr;

    const GameObject * mObject = nullptr;

    const Cell2D mTarget;
};

} // namespace game
