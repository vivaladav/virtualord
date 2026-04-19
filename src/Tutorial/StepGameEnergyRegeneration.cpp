#include "Tutorial/StepGameEnergyRegeneration.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameEnergyRegeneration::StepGameEnergyRegeneration()
    : TutorialInfoStep(600, 325)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1250, 500);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_ENERGY_REGENERATION_1"),
                       TutorialConstants::colorText, 6.f, true, false);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_ENERGY_REGENERATION_2"),
                       TutorialConstants::colorText, 7.f, true, false);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_ENERGY_REGENERATION_3"),
                       TutorialConstants::colorText, 11.f, true, false);

    info->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

} // namespace game
