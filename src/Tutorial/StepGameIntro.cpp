#include "Tutorial/StepGameIntro.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

namespace game
{

StepGameIntro::StepGameIntro()
{
    mInfo = new PanelInfoTutorial(500, 250);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(200, 100);

    mInfo->AddInfoEntry("Welcome commander,",
                        colorTutorialText, 2.5f, true, false);
    mInfo->AddInfoEntry("I am your field assistant and I am going to guide you during your first mission.",
                        colorTutorialText, 0.f, true, false);

    mInfo->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

void StepGameIntro::OnStart()
{
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    mInfo->StartInfo();
}

void StepGameIntro::OnEnd()
{
    delete mInfo;
}

} // namespace game
