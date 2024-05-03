#include "Tutorial/StepGameMapNavigation.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

namespace game
{

StepGameMapNavigation::StepGameMapNavigation()
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // INFO
    mInfo = new PanelInfoTutorial(650, 400);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(1200, 450);

    mInfo->AddInfoEntry("Now that you learned how to control your units I want to show you how to move around.",
                        colorTutorialText, 8.f, true, true);
    mInfo->AddInfoEntry("You can navigate the map in 3 different ways:", colorTutorialText, 5.f, true, false);
    mInfo->AddInfoEntry("- You can move your mouse to the border of the screen and the map will scroll in that "
                        "direction.", colorTutorialText, 12.f, true, false);
    mInfo->AddInfoEntry("- You can use the keyboard keys W, A, S, D to move in the 4 directions.",
                        colorTutorialText, 10.f, true, false);
    mInfo->AddInfoEntry("- You can click with your LEFT MOUSE button on it and drag to move in the opposite "
                        "direction.", colorTutorialText, 10.f, true, false);
    mInfo->AddInfoEntry("Now I will let you to practice and to explore this map.",
                        colorTutorialText, 9.f, true, false);

    mInfo->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

StepGameMapNavigation::~StepGameMapNavigation()
{
    delete mClickFilter;
    delete mInfo;
}

void StepGameMapNavigation::OnStart()
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
