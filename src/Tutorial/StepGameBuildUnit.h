#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class GameHUD;
class FocusArea;
class Unit;

class StepGameBuildUnit : public TutorialInfoStep
{
public:
    StepGameBuildUnit(GameHUD * HUD, const char * textUnit, const char * textBuild, int unitInd);
    ~StepGameBuildUnit();

    void SetDelayAnimation(float val);

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;

    GameHUD * mHUD = nullptr;
    int mIndStruct = 0;
    unsigned int mButtonUnitId = 0;
    unsigned int mButtonBuildId = 0;
    float mDelayAnim = 0.5f;
};

inline void StepGameBuildUnit::SetDelayAnimation(float val)
{
    mDelayAnim = val;
}

} // namespace game
