#include "Tutorial/StepGameIntro3.h"

#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameIntro3::StepGameIntro3()
    : TutorialInfoStep(550, 200)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(175, 200);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_INTRO_5"), 7.f, true, false);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_INTRO_4"), 7.f, true, false);

    info->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

} // namespace game
