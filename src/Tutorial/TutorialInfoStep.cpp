#include "Tutorial/TutorialInfoStep.h"

#include "Screens/ScreenGame.h"

#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <cassert>

namespace game
{

TutorialInfoStep::TutorialInfoStep(int infoW, int infoH)
: TutorialInfoStep(nullptr, infoW, infoH)
{
}

TutorialInfoStep::TutorialInfoStep(Screen * screen, int infoW, int infoH)
    : mInfo(new PanelInfoTutorial(infoW, infoH))
    , mScreen(screen)
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

    // disable selection in ScreenGame, if requested
    if(mDisableSelection)
    {
        auto screen = dynamic_cast<ScreenGame *>(mScreen);
        assert(screen);

        // store selection allowed in screen
        mWasSelAllowed = screen->IsSelectionAllowed();
        // disable selection allowed in screen to avoid to select random objects
        screen->SetSelectionAllowed(false);
    }

    // INFO
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    mInfo->StartInfo();
}

void TutorialInfoStep::OnEnd()
{
    TutorialStep::OnEnd();

    // restore selection allowed as before starting, if requested
    if(mDisableSelection)
    {
        auto screen = dynamic_cast<ScreenGame *>(mScreen);
        assert(screen);

        screen->SetSelectionAllowed(mWasSelAllowed);
    }
}

} // namespace game
