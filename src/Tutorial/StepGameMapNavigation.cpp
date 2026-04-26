#include "Tutorial/StepGameMapNavigation.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameMapNavigation::StepGameMapNavigation()
    : TutorialInfoStep(750, 300)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1100, 400);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_MAP_NAVIGATION_1"),
                       TutorialConstants::colorText, 9.f, true, true);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_MAP_NAVIGATION_2"),
                       TutorialConstants::colorText, 5.f, true, false);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_MAP_NAVIGATION_3"),
                       TutorialConstants::colorText, 12.f, true, false);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_MAP_NAVIGATION_4"),
                       TutorialConstants::colorText, 10.f, true, false);

    info->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

} // namespace game
