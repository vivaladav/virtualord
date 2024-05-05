#include "Tutorial/StepGameStructDisconnected.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

namespace game
{

StepGameStructDisconnected::StepGameStructDisconnected()
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // INFO
    mInfo = new PanelInfoTutorial(550, 350);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(1300, 250);

    mInfo->AddInfoEntry("Okay, enough of that for now. Let's go back to your conquest!",
                        colorTutorialText, 5.f, true, true);
    mInfo->AddInfoEntry("You might have noticed the blinking icon on top of the energy generator.",
                        colorTutorialText, 6.f, true, false);
    mInfo->AddInfoEntry("That means that this structire is not connected to your base yet, hence "
                        "it's not active.", colorTutorialText, 7.f, true, false);
    mInfo->AddInfoEntry("To connect it to your base you need to create a path of conquered cells "
                        "with your unit.", colorTutorialText, 10.f, true, false);

    mInfo->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

StepGameStructDisconnected::~StepGameStructDisconnected()
{
    delete mClickFilter;
    delete mInfo;
}

void StepGameStructDisconnected::OnStart()
{
    // CLICK FILTER
    mClickFilter->SetEnabled(true);

    // INFO
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    mInfo->StartInfo();
}

} // namespace game
