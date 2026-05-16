#pragma once

#include "Tutorial/TutorialInfoStep.h"

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

class StepGameConquerStruct : public TutorialInfoStep
{
public:
    StepGameConquerStruct(const Game * game, const Player * p, const GameObject * energyGen,
                          const IsoMap * isoMap);
    ~StepGameConquerStruct();

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;

    const GameObject * mEnergyGen = nullptr;
    Unit * mUnit = nullptr;
};

} // namespace game
