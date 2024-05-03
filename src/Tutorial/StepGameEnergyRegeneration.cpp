#include "Tutorial/StepGameEnergyRegeneration.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

namespace game
{

StepGameEnergyRegeneration::StepGameEnergyRegeneration()
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // INFO
    mInfo = new PanelInfoTutorial(550, 325);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(200, 600);

    mInfo->AddInfoEntry("As you can see now all your faction energy is restored.",
                        colorTutorialText, 5.f, true, false);
    mInfo->AddInfoEntry("Your unit's energy is fully restored too, but it's not always the case.",
                        colorTutorialText, 6.f, true, false);
    mInfo->AddInfoEntry("The amount restored depends on its REGENERATION attribute and by how much "
                        "it was left when you ended your turn.", colorTutorialText, 10.f, true, false);

    mInfo->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

StepGameEnergyRegeneration::~StepGameEnergyRegeneration()
{
    delete mClickFilter;
    delete mInfo;
}

void StepGameEnergyRegeneration::OnStart()
{
    // CLICK FILTER
    mClickFilter->SetEnabled(true);

    // INFO
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    mInfo->StartInfo();
}

} // namespace game
