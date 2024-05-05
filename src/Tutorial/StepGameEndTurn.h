#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class FocusArea;
class PanelClickFilter;
class PanelInfoTutorial;
class PanelTurnControl;

class StepGameEndTurn : public TutorialStep
{
public:
    StepGameEndTurn(const game::PanelTurnControl * panel);
    ~StepGameEndTurn();

    void OnStart() override;

private:
    FocusArea * mFocusArea = nullptr;
    PanelClickFilter * mClickFilter = nullptr;
    PanelInfoTutorial * mInfo = nullptr;
};

} // namespace game
