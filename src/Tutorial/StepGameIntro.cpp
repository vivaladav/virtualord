#include "Tutorial/StepGameIntro.h"

#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameIntro::StepGameIntro()
    : TutorialInfoStep(550, 250)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(200, 100);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_INTRO_1"), 6.f, true, false);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_INTRO_2"), 7.f, true, false);

    info->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

} // namespace game
