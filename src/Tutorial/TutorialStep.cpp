#include "TutorialStep.h"

#include "Widgets/Tutorial/PanelClickFilter.h"

namespace game
{

TutorialStep::TutorialStep()
    : mClickFilter(new PanelClickFilter)
{
    mClickFilter->SetEnabled(false);
}

TutorialStep::~TutorialStep()
{
    delete mClickFilter;
}

void TutorialStep::SetPause(bool paused)
{
    mPaused = paused;
}

void TutorialStep::OnStart()
{
    mClickFilter->SetEnabled(true);
}

void TutorialStep::Update(float) { }

} // namespace game
