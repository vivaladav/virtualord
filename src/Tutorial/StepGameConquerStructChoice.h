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

class StepGameConquerStructChoice : public TutorialInfoStep
{
public:
    StepGameConquerStructChoice(const Game * game, const GameObject * unit,
                                const GameObject * struct1, const GameObject * struct2,
                                const IsoMap * isoMap);
    ~StepGameConquerStructChoice();

    void Update(float delta) override;

private:
    FocusArea * mFocusArea1 = nullptr;
    FocusArea * mFocusArea2 = nullptr;

    const GameObject * mStruct1 = nullptr;
    const GameObject * mStruct2 = nullptr;
    const GameObject * mUnit = nullptr;
};

} // namespace game
