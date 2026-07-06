#include "Tutorial/StepPlanetMapExploreTerritoryInfo.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepPlanetMapExploreTerritoryInfo::StepPlanetMapExploreTerritoryInfo()
    : TutorialInfoStep(TutorialConstants::infoPlanetMapW, TutorialConstants::infoPlanetMapH)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(TutorialConstants::infoPlanetMapX, TutorialConstants::infoPlanetMapY);

    info->AddInfoEntry(sm->GetCString("TUT_PM_EXPLORE_TERRITORY_INFO_1"), 7.f, true, true);
    info->AddInfoEntry(sm->GetCString("TUT_PM_EXPLORE_TERRITORY_INFO_2"), 7.f, true, true);

    info->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

} // namespace game
