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
    mInfo = new PanelInfoTutorial(infoPlanetMapW, infoPlanetMapH);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(infoPlanetMapX, infoPlanetMapY);

    mInfo->AddInfoEntry("Welcome commander,",
                        colorTutorialText, 3.5f, true, true);
    mInfo->AddInfoEntry("I am your assistant and I will guide you in your first planet conquest.",
                        colorTutorialText, 6.f, true, true);
    mInfo->AddInfoEntry("This is the planet screen, where you can decide how to manage your expansion.",
                        colorTutorialText, 7.f, true, true);

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
