#include "Tutorial/StepGameStructConnected.h"

#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameStructConnected::StepGameStructConnected()
    : TutorialInfoStep(750, 300)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1150, 450);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_STRUCT_CONNECTED_1"), 10.f, true, false);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_STRUCT_CONNECTED_2"), 12.f, true, false);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_STRUCT_CONNECTED_3"), 13.f, true, false);

    info->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

} // namespace game
