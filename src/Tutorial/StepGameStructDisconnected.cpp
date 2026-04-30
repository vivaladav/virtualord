#include "Tutorial/StepGameStructDisconnected.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameStructDisconnected::StepGameStructDisconnected()
    : TutorialInfoStep(600, 320)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1250, 250);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_STRUCT_DISCONNECTED_1"),
                       TutorialConstants::colorText, 6.f, true, true);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_STRUCT_DISCONNECTED_2"),
                       TutorialConstants::colorText, 7.f, true, false);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_STRUCT_DISCONNECTED_3"),
                       TutorialConstants::colorText, 7.f, true, false);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_STRUCT_DISCONNECTED_4"),
                       TutorialConstants::colorText, 11.f, true, false);

    info->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

} // namespace game
