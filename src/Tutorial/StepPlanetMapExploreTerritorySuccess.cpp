#include "Tutorial/StepPlanetMapExploreTerritorySuccess.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelPlanetActionExplore.h"
#include "Widgets/PanelPlanetInfo.h"
#include "Widgets/PanelPlanetResources.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>

namespace game
{

StepPlanetMapExploreTerritorySuccess::StepPlanetMapExploreTerritorySuccess(const PanelPlanetActionExplore * panelExplore,
                                                                           const PanelPlanetInfo * panelInfo,
                                                                           const PanelPlanetResources * panelResources)
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

    const int fX1 = panelInfo->GetX() + padding;
    const int fY1 = panelInfo->GetY() + padding;
    const int fW1 = panelInfo->GetWidth() - (padding * 2);
    const int fH1 = panelInfo->GetHeight() - (padding * 2);

    mFocusInfo = new FocusArea;
    mFocusInfo->SetScreenArea(fX1, fY1, fW1, fH1);
    mFocusInfo->SetCornersColor(colorTutorialFocusElement);
    mFocusInfo->SetVisible(false);

    const int fX2 = panelResources->GetX() + padding;
    const int fY2 = panelResources->GetY() + padding;
    const int fW2 = panelResources->GetWidth() - (padding * 2);
    const int fH2 = panelResources->GetHeight() - (padding * 2);

    mFocusResources = new FocusArea;
    mFocusResources->SetScreenArea(fX2, fY2, fW2, fH2);
    mFocusResources->SetCornersColor(colorTutorialFocusElement);
    mFocusResources->SetVisible(false);

    // INFO
    mInfo = new PanelInfoTutorial(infoPlanetMapW, infoPlanetMapH);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(infoPlanetMapX, infoPlanetMapY);

    mInfo->AddInfoEntry("Exploration was successful and now you can see several detailed "
                        "info in the RESOURCES and INFO panels.", colorTutorialText, 7.f, true, true);
    mInfo->AddInfoEntry("Click the button CLOSE to go back to the actions panel.",
                        colorTutorialTextAction, 0.f, false, false, [this, fX, fY, fW, fH]
                        {
                            mClickFilter->SetScreenClickableArea(fX, fY, fW, fH);

                            mFocusInfo->SetVisible(false);
                            mFocusResources->SetVisible(false);

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

    // FOCUS
    mFocusInfo->SetVisible(true);
    mFocusResources->SetVisible(true);

    // INFO
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    mInfo->StartInfo();
}

} // namespace game
