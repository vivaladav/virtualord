#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class GameObject;
class IsoMap;
class PanelClickFilter;
class PanelInfoTutorial;
class Player;
class Unit;

class StepGameConquerStructSimple : public TutorialInfoStep
{
public:
    StepGameConquerStructSimple(const Player * p, const GameObject * energyGen, const IsoMap * isoMap);
    ~StepGameConquerStructSimple();

    void Update(float delta) override;

private:
    FocusArea * mFocusArea = nullptr;

    const GameObject * mStruct = nullptr;
    Unit * mUnit = nullptr;
};

} // namespace game
