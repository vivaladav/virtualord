#include "Tutorial/StepPlanetMapExploreTerritoryStart.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelPlanetActionExplore.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>

namespace game
{

StepPlanetMapExploreTerritoryStart::StepPlanetMapExploreTerritoryStart(PanelPlanetActionExplore * panelExplore)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // FOCUS
    auto btnExplore = panelExplore->GetButtonOk();

    const int padding = 10;
    const int fX = panelExplore->GetX() + btnExplore->GetX() - padding;
    const int fY = panelExplore->GetY() + btnExplore->GetY() - padding;
    const int fW = btnExplore->GetWidth() + (padding * 2);
    const int fH = btnExplore->GetHeight() + (padding * 2);

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

    mInfo->AddInfoEntry("Now click the button PROCEED to start the exploration.",
                        colorTutorialTextAction, 0.f, false, false, [this, fX, fY, fW, fH]
                        {
                            mClickFilter->SetScreenClickableArea(fX, fY, fW, fH);
                        });

    // EXPLORE BUTTON
    btnExplore->AddOnClickFunction([this]
    {
        SetDone();
    });
}

StepPlanetMapExploreTerritoryStart::~StepPlanetMapExploreTerritoryStart()
{
    delete mClickFilter;
    delete mFocusArea;
    delete mInfo;
}

void StepPlanetMapExploreTerritoryStart::OnStart()
{
    // CLICK FILTER
    mClickFilter->SetEnabled(true);

    // FOCUS
    mFocusArea->SetVisible(true);

    // INFO
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    mInfo->StartInfo();
}

} // namespace game
