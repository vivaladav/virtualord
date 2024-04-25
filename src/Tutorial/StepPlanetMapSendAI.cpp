#include "Tutorial/StepPlanetMapSendAI.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelPlanetActions.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>

namespace game
{

StepPlanetMapSendAI::StepPlanetMapSendAI(PanelPlanetActions * panel)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // FOCUS
    auto btn = panel->GetButton(PanelPlanetActions::SEND_AI);

    const int padding = 10;
    const int fX = panel->GetX() + btn->GetX() - padding;
    const int fY = panel->GetY() + btn->GetY() - padding;
    const int fW = btn->GetWidth() + (padding * 2);
    const int fH = btn->GetHeight() + (padding * 2);

    mFocusArea = new FocusArea;
    mFocusArea->SetScreenArea(fX, fY, fW, fH);
    mFocusArea->SetCornersColor(colorTutorialFocusElement);
    mFocusArea->SetVisible(false);

    // INFO
    mInfo = new PanelInfoTutorial(infoPlanetMapW, infoPlanetMapH);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(infoPlanetMapX, infoPlanetMapY);

    mInfo->AddInfoEntry("Now that the territory is explored you can decide if you want to conquer it or not.",
                        colorTutorialText, 5.f, true, true);
    mInfo->AddInfoEntry("You can send an AI general to do the job for you, but it will cost you a good amount"
                        " of resources and victory is never guaranteed.",
                        colorTutorialText, 7.f, true, true, [this]
                        {
                            mFocusArea->SetVisible(true);
                        });

    mInfo->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

StepPlanetMapSendAI::~StepPlanetMapSendAI()
{
    delete mClickFilter;
    delete mFocusArea;
    delete mInfo;
}

void StepPlanetMapSendAI::OnStart()
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
