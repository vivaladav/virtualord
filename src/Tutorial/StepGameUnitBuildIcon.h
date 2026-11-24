#pragma once

#include "GameObjects/GameObjectTypes.h"
#include "Tutorial/TutorialInfoStep.h"

namespace game
{

class FocusArea;
class Game;
class PanelObjectActions;

class StepGameUnitBuildIcon : public TutorialInfoStep
{
public:
    StepGameUnitBuildIcon(const Game * game, Screen * screen, PanelObjectActions * panel,
                          const char * text, GameObjectTypeId typeBuild, float animDelay = 0.5f);
    ~StepGameUnitBuildIcon();

    void SetPause(bool paused) override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelObjectActions * mPanelActions = nullptr;

    unsigned int mClickId = 0;
};

} // namespace game
