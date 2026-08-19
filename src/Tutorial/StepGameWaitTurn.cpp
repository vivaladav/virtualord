#include "Tutorial/StepGameWaitTurn.h"

#include "Screens/ScreenGame.h"

namespace game
{

StepGameWaitTurn::StepGameWaitTurn(const ScreenGame * screen)
    : mScreen(screen)
{
}


void StepGameWaitTurn::Update(float delta)
{
    TutorialStep::Update(delta);

    if(mScreen->IsCurrentTurnLocal())
        SetDone();
}

} // namespace game
