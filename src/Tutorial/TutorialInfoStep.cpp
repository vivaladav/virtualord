#include "Tutorial/TutorialInfoStep.h"

#include "Widgets/Tutorial/PanelInfoTutorial.h"

namespace game
{

TutorialInfoStep::TutorialInfoStep(int infoW, int infoH)
    : mInfo(new PanelInfoTutorial(infoW, infoH))
{
    // INFO
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
}

TutorialInfoStep::~TutorialInfoStep()
{
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

    // INFO
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    mInfo->StartInfo();
}

} // namespace game
