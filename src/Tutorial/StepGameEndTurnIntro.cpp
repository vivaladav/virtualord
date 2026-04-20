#include "Tutorial/StepGameEndTurnIntro.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameEndTurnIntro::StepGameEndTurnIntro()
    : TutorialInfoStep(550, 150)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1250, 200);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_END_TURN"),
                       TutorialConstants::colorText, 8.f, true, false);

    info->SetFunctionOnFinished([this]
                                {
                                    SetDone();
                                });
}

} // namespace game
