#pragma once

#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class Game;
class PanelObjectActions;
class ScreenGame;

class StepGameBaseBuildUnitIcon : public TutorialInfoStep
{
public:
    StepGameBaseBuildUnitIcon(const Game * game, ScreenGame * screen, PanelObjectActions * panel);
    ~StepGameBaseBuildUnitIcon();

    void OnStart() override;
    void OnEnd() override;

private:
    FocusArea * mFocusArea = nullptr;
    ScreenGame * mScreen = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;

    bool mSelAllowed;
};

} // namespace game
