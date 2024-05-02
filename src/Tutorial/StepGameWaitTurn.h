#pragma once

#include "Tutorial/TutorialStep.h"

namespace game
{

class PanelClickFilter;
class ScreenGame;

class StepGameWaitTurn : public TutorialStep
{
public:
    StepGameWaitTurn(const ScreenGame * screen);
    ~StepGameWaitTurn();

    void OnStart() override;

    void Update(float delta) override;

private:
    PanelClickFilter * mClickFilter = nullptr;

    const ScreenGame * mScreen = nullptr;
};

} // namespace game
