#include "Tutorial/StepDelay.h"

#include "Widgets/Tutorial/PanelClickFilter.h"

namespace game
{

StepDelay::StepDelay(float sec)
    : mTimer(sec)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);
}

StepDelay::~StepDelay()
{
    delete mClickFilter;
}

void StepDelay::OnStart()
{
    // CLICK FILTER
    mClickFilter->SetEnabled(true);
}

void StepDelay::Update(float delta)
{
    mTimer -= delta;

    if(mTimer <= 0.f)
        SetDone();
}

} // namespace game
