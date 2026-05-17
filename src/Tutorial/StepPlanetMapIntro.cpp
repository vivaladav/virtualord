#include "Tutorial/StepPlanetMapIntro.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepPlanetMapIntro::StepPlanetMapIntro(bool won)
    : TutorialInfoStep(TutorialConstants::infoPlanetMapW, TutorialConstants::infoPlanetMapH)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(TutorialConstants::infoPlanetMapX, TutorialConstants::infoPlanetMapY);

    if(won)
        info->AddInfoEntry(sm->GetCString("TUT_PM_INTRO_1"), 5.f, true, true);
    else
        info->AddInfoEntry(sm->GetCString("TUT_PM_INTRO_2"), 7.f, true, true);

    info->AddInfoEntry(sm->GetCString("TUT_PM_INTRO_3"), 8.f, true, true);

    info->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

} // namespace game
