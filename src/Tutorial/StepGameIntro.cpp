#include "Tutorial/StepGameIntro.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/core/Timer.h>

namespace game
{

StepGameIntro::StepGameIntro()
{
    mInfo = new PanelInfoTutorial(500, 250);
    mInfo->SetPosition(200, 100);
    mInfo->SetFocus();

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
    mInfo->StartInfo();
}

void StepGameIntro::OnEnd()
{
    delete mInfo;
}

} // namespace game
