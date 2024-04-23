#include "Tutorial/StepPlanetMapNoInfo.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>

namespace game
{

StepPlanetMapNoInfo::StepPlanetMapNoInfo()
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // INFO
    mInfo = new PanelInfoTutorial(800, 125);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(560, 100);

    mInfo->AddInfoEntry("You selected the territory, but the INFO and RESOURCES panels "
                        "do not show any data yet.", colorTutorialText, 6.5f, true, true);
    mInfo->AddInfoEntry("That's because this territory is still unexplored.",
                        colorTutorialText, 4.f, true, true);

    mInfo->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

StepPlanetMapNoInfo::~StepPlanetMapNoInfo()
{
    delete mClickFilter;
    delete mInfo;
}

void StepPlanetMapNoInfo::OnStart()
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
