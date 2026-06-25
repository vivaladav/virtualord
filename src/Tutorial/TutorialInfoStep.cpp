#include "Tutorial/TutorialInfoStep.h"

#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

namespace game
{

TutorialInfoStep::TutorialInfoStep(int infoW, int infoH)
    : mClickFilter(new PanelClickFilter)
    , mInfo(new PanelInfoTutorial(infoW, infoH))
{
    // CLICK FILTER
    mClickFilter->SetEnabled(false);

    // INFO
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
}

TutorialInfoStep::~TutorialInfoStep()
{
    delete mClickFilter;
    delete mInfo;
}

void TutorialInfoStep::SetPause(bool paused)
{
    TutorialStep::SetPause(paused);

    mInfo->SetEnabled(!paused);

    // give focus back to info panel when tutorial is resumed
    if(!paused)
        mInfo->SetFocus();
}

void TutorialInfoStep::OnStart()
{
    TutorialStep::OnStart();

    // CLICK FILTER
    mClickFilter->SetEnabled(true);

    // INFO
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    mInfo->StartInfo();
}

} // namespace game
