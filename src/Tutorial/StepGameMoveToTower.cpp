#include "Tutorial/StepGameMoveToTower.h"

#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameMoveToTower::StepGameMoveToTower()
    : TutorialInfoStep(550, 150)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1000, 250);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_MAP_NAVIGATION_5"), 10.f, true, false);

    info->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

} // namespace game
