#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class GameHUD;
class FocusArea;

class StepGameBuildStructure : public TutorialInfoStep
{
public:
    StepGameBuildStructure(GameHUD * HUD, const char * textCat, const char * textStruct,
                           int indCat, int indStruct);
    ~StepGameBuildStructure();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;

    GameHUD * mHUD = nullptr;
    int mIndCat = 0;
    int mIndStruct = 0;
    unsigned int mButtonCatId = 0;
    unsigned int mButtonStructId = 0;
    unsigned int mButtonBuildId = 0;
};

} // namespace game
