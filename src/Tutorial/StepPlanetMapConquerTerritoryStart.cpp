#include "Tutorial/StepPlanetMapConquerTerritoryStart.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelPlanetActionConquer.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>

namespace game
{

StepPlanetMapConquerTerritoryStart::StepPlanetMapConquerTerritoryStart(PanelPlanetActionConquer * panel)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // FOCUS
    auto btn = panel->GetButtonOk();

    const int padding = 10;
    const int fX = panel->GetX() + btn->GetX() - padding;
    const int fY = panel->GetY() + btn->GetY() - padding;
    const int fW = btn->GetWidth() + (padding * 2);
    const int fH = btn->GetHeight() + (padding * 2);

    mFocusArea = new FocusArea;
    mFocusArea->SetScreenArea(fX, fY, fW, fH);
    mFocusArea->SetCornersColor(colorTutorialFocusAction);
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    mInfo = new PanelInfoTutorial(infoPlanetMapW, infoPlanetMapH);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(infoPlanetMapX, infoPlanetMapY);

    mInfo->AddInfoEntry("Now click the button PROCEED to start your first mission.",
                        colorTutorialTextAction, 0.f, false, false);

    mInfo->SetFunctionOnFinished([this, fX, fY, fW, fH]
    {
        // CLICK FILTER
        mClickFilter->SetScreenClickableArea(fX, fY, fW, fH);
        mClickFilter->SetEnabled(true);

        // FOCUS
        mFocusArea->SetVisible(true);
    });

    // CONQUER BUTTON
    btn->AddOnClickFunction([this]
    {
        SetDone();
    });
}

StepPlanetMapConquerTerritoryStart::~StepPlanetMapConquerTerritoryStart()
{
    delete mClickFilter;
    delete mFocusArea;
    delete mInfo;
}

void StepPlanetMapConquerTerritoryStart::OnStart()
{
    // INFO
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    mInfo->StartInfo();
}

} // namespace game
