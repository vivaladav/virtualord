#include "Tutorial/StepPlanetMapIntro.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

namespace game
{

StepPlanetMapIntro::StepPlanetMapIntro()
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->ClearClickableArea();

    // INFO
    mInfo = new PanelInfoTutorial(500, 200);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(150, 850);

    mInfo->AddInfoEntry("Welcome commander,",
                        colorTutorialText, 2.5f, true, false);
    mInfo->AddInfoEntry("I am your assistant and I will guide you in your first planet conquest.",
                        colorTutorialText, 0.f, true, false);

    mInfo->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

StepPlanetMapIntro::~StepPlanetMapIntro()
{
    delete mClickFilter;
    delete mInfo;
}

void StepPlanetMapIntro::OnStart()
{
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    mInfo->StartInfo();
}

} // namespace game
