#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class FocusArea;
class Game;
class GameObject;
class IsoFocusArea;
class IsoMap;
class Player;
class Unit;

class StepGameOpenLootbox : public TutorialInfoStep
{
public:
    StepGameOpenLootbox(const Game * game, const Unit * unit, const GameObject * lootbox,
                        const IsoMap * isoMap, const char * intro, const sgl::core::Pointd2D & p0);
    ~StepGameOpenLootbox();

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;
    IsoFocusArea * mIsoFocusArea = nullptr;

    const GameObject * mLootbox = nullptr;
    const Unit * mUnit = nullptr;
};

} // namespace game
