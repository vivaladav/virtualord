#include "Tutorial/StepPlanetMapExploreTerritorySuccess.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelPlanetActionExplore.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>

namespace game
{

StepPlanetMapExploreTerritorySuccess::StepPlanetMapExploreTerritorySuccess(PanelPlanetActionExplore * panelExplore)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // FOCUS
    auto btnClose = panelExplore->GetButtonCancel();

    const int padding = 10;
    const int fX = panelExplore->GetX() + btnClose->GetX() - padding;
    const int fY = panelExplore->GetY() + btnClose->GetY() - padding;
    const int fW = btnClose->GetWidth() + (padding * 2);
    const int fH = btnClose->GetHeight() + (padding * 2);

    mFocusArea = new FocusArea;
    mFocusArea->SetScreenArea(fX, fY, fW, fH);
    mFocusArea->SetCornersColor(colorTutorialFocusAction);
    mFocusArea->SetVisible(false);

    // INFO
    mInfo = new PanelInfoTutorial(infoPlanetMapW, infoPlanetMapH);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(infoPlanetMapX, infoPlanetMapY);

    mInfo->AddInfoEntry("Exploration was successful and now you can see all the detailed "
                        "info in the RESOURCES and INFO panels.", colorTutorialText, 5.f, true, true);
    mInfo->AddInfoEntry("Click the button CLOSE to go back to the actions panel.",
                        colorTutorialTextAction, 0.f, false, false);

    mInfo->SetFunctionOnFinished([this, fX, fY, fW, fH]
    {
        mClickFilter->SetScreenClickableArea(fX, fY, fW, fH);

        mFocusArea->SetVisible(true);
    });

    // EXPLORE BUTTON
    btnClose->AddOnClickFunction([this]
    {
        SetDone();
    });
}

StepPlanetMapExploreTerritorySuccess::~StepPlanetMapExploreTerritorySuccess()
{
    delete mClickFilter;
    delete mFocusArea;
    delete mInfo;
}

void StepPlanetMapExploreTerritorySuccess::OnStart()
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
