#include "Tutorial/StepGameConnectStructIntro.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameConnectStructIntro::StepGameConnectStructIntro()
    : TutorialInfoStep(550, 150)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1250, 200);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_CONNECT_STRUCT"),
                       TutorialConstants::colorText, 8.f, true, false);

    info->SetFunctionOnFinished([this]
                                {
                                    SetDone();
                                });
}

} // namespace game
