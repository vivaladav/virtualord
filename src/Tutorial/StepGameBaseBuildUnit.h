#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class GameHUD;
class FocusArea;
class PanelClickFilter;
class PanelInfoTutorial;

class StepGameBaseBuildUnit : public TutorialStep
{
public:
    StepGameBaseBuildUnit(GameHUD * HUD);
    ~StepGameBaseBuildUnit();

    void OnStart() override;
    void OnEnd() override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;

    GameHUD * mHUD = nullptr;
    unsigned int mButtonActId;
};

} // namespace game
