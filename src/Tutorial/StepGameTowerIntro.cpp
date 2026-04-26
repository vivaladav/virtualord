#include "Tutorial/StepGameTowerIntro.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameTowerIntro::StepGameTowerIntro()
    : TutorialInfoStep(550, 150)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1100, 550);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_TOWER_INTRO"),
                       TutorialConstants::colorText, 10.f, true, false);


    info->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

} // namespace game
