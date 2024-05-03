#include "Tutorial/StepGameStructConnected.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

namespace game
{

StepGameStructConnected::StepGameStructConnected()
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // INFO
    mInfo = new PanelInfoTutorial(550, 300);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(1300, 200);

    mInfo->AddInfoEntry("Well done commander!", colorTutorialText, 4.f, true, true);
    mInfo->AddInfoEntry("Now the energy generator is connected to your base and from now on you "
                        "will receive energy from it every turn.", colorTutorialText, 9.f, true, false);
    mInfo->AddInfoEntry("Connecting structures to your base will also grow your influence area, which is"
                        "delimited by the colored line surrounding all this area.", colorTutorialText,
                        10.f, true, false);

    mInfo->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

StepGameStructConnected::~StepGameStructConnected()
{
    delete mClickFilter;
    delete mInfo;
}

void StepGameStructConnected::OnStart()
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
