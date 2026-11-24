#pragma once

#include "GameObjects/GameObjectTypes.h"
#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class GameHUD;
class FocusArea;
class Unit;

class StepGameBuildStructure : public TutorialInfoStep
{
public:
    StepGameBuildStructure(const Unit * unit, GameHUD * HUD, const char * textCat,
                           const char * textStruct, const char * textBuild, GameObjectTypeId type);
    ~StepGameBuildStructure();

    void SetDelayAnimation(float val);

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;

    GameHUD * mHUD = nullptr;
    int mIndCat = 0;
    int mIndStruct = 0;
    unsigned int mButtonCatId = 0;
    unsigned int mButtonStructId = 0;
    unsigned int mButtonBuildId = 0;
    float mDelayAnim = 0.5f;
};

inline void StepGameBuildStructure::SetDelayAnimation(float val)
{
    mDelayAnim = val;
}

} // namespace game
