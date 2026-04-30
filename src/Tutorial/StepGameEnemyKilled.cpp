#include "Tutorial/StepGameEnemyKilled.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameEnemyKilled::StepGameEnemyKilled()
    : TutorialInfoStep(550, 150)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1100, 500);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_ENEMY1_KILLED"),
                       TutorialConstants::colorText, 10.f, true, false);

    info->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

} // namespace game
