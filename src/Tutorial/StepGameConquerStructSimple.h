#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class FocusArea;
class Game;
class GameObject;
class IsoMap;
class PanelClickFilter;
class PanelInfoTutorial;
class Player;
class Unit;

class StepGameConquerStructSimple : public TutorialInfoStep
{
public:
    StepGameConquerStructSimple(const Game *game, const Unit * unit, const GameObject * energyGen,
                                const IsoMap * isoMap, const sgl::core::Pointd2D & p0);
    ~StepGameConquerStructSimple();

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;

    const GameObject * mStruct = nullptr;
    const Unit * mUnit = nullptr;
};

} // namespace game
