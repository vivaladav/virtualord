#include "Tutorial/StepGameEnding.h"

#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameEnding::StepGameEnding(const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(650, 150)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_END_1"), 6.f, true, true);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_END_2"), 10.f, true, true);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_END_3"), 6.f, true, true);

    info->SetFunctionOnFinished([this]
                                {
                                    SetDone();
                                });
}

} // namespace game
