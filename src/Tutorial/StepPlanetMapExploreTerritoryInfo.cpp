#include "Tutorial/StepPlanetMapExploreTerritoryInfo.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>

namespace game
{

StepPlanetMapExploreTerritoryInfo::StepPlanetMapExploreTerritoryInfo()
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // INFO
    mInfo = new PanelInfoTutorial(infoPlanetMapW, infoPlanetMapH);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(infoPlanetMapX, infoPlanetMapY);

    mInfo->AddInfoEntry("As you can see exploring a territory requires to spend some resources.",
                        colorTutorialText, 5.f, true, true);
    mInfo->AddInfoEntry("Usually It's worth it as it reveals all the important details.",
                        colorTutorialText, 5.f, true, true);

    mInfo->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

StepPlanetMapExploreTerritoryInfo::~StepPlanetMapExploreTerritoryInfo()
{
    delete mClickFilter;
    delete mInfo;
}

void StepPlanetMapExploreTerritoryInfo::OnStart()
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
