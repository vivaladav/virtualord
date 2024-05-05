#include "Tutorial/StepGameWaitTurn.h"

#include "Screens/ScreenGame.h"
#include "Widgets/Tutorial/PanelClickFilter.h"

namespace game
{

StepGameWaitTurn::StepGameWaitTurn(const ScreenGame * screen)
    : mScreen(screen)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);
}

StepGameWaitTurn::~StepGameWaitTurn()
{
    delete mClickFilter;
}

void StepGameWaitTurn::OnStart()
{
    // CLICK FILTER
    mClickFilter->SetEnabled(true);
}

void StepGameWaitTurn::Update(float delta)
{
    if(mScreen->IsCurrentTurnLocal())
        SetDone();
}

} // namespace game
