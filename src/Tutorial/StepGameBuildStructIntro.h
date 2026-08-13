#pragma once

#include "Tutorial/TutorialInfoStep.h"

#include <sgl/core/Point.h>

namespace game
{

class FocusArea;
class Game;
class PanelObjectActions;
class ScreenGame;

class StepGameBuildStructIntro : public TutorialInfoStep
{
public:
    StepGameBuildStructIntro(const Game * game, ScreenGame * screen, PanelObjectActions * panel,
                             const char * textIntro, const sgl::core::Pointd2D & p0);
    ~StepGameBuildStructIntro();

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
