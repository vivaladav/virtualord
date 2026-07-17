#include "Tutorial/StepGameSetSelectionAllowed.h"

#include "Screens/ScreenGame.h"

namespace game
{

StepGameSetSelectionAllowed::StepGameSetSelectionAllowed(ScreenGame * screen, bool allowed)
    : mScreen(screen)
    , mAllowed(allowed)
{
}

void StepGameSetSelectionAllowed::OnStart()
{
    TutorialStep::OnStart();

    mScreen->SetSelectionAllowed(mAllowed);

    SetDone();
}

} // namespace game
