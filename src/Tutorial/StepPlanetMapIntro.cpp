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
    mClickFilter->SetEnabled(false);

    // INFO
    mInfo = new PanelInfoTutorial(800, 125);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(560, 100);

    mInfo->AddInfoEntry("Welcome commander,",
                        colorTutorialText, 3.f, true, true);
    mInfo->AddInfoEntry("I am your assistant and I will guide you in your first planet conquest.",
                        colorTutorialText, 0.f, true, true);
    mInfo->AddInfoEntry("This is the planet map screen.",
                        colorTutorialText, 3.f, true, true);
    mInfo->AddInfoEntry("From here you can decide which territories to conquest and how to do it.",
                        colorTutorialText, 0.f, true, true);

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
    // CLICK FILTER
    mClickFilter->SetEnabled(true);

    // INFO
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    mInfo->StartInfo();
}

} // namespace game
