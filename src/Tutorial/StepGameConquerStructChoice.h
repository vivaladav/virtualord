#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class Game;
class GameObject;
class IsoFocusArea;
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
    IsoFocusArea * mFocusArea1 = nullptr;
    IsoFocusArea * mFocusArea2 = nullptr;

    const GameObject * mStruct1 = nullptr;
    const GameObject * mStruct2 = nullptr;
    const GameObject * mUnit = nullptr;
};

} // namespace game
