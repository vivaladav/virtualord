#include "Tutorial/StepPlanetMapExploreTerritorySuccess.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelPlanetActionExplore.h"
#include "Widgets/PanelPlanetInfo.h"
#include "Widgets/PanelPlanetResources.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepPlanetMapExploreTerritorySuccess::StepPlanetMapExploreTerritorySuccess(const PanelPlanetActionExplore * panelExplore,
                                                                           const PanelPlanetInfo * panelInfo,
                                                                           const PanelPlanetResources * panelResources)
    : TutorialInfoStep(TutorialConstants::infoPlanetMapW, TutorialConstants::infoPlanetMapH)
    , mFocusArea(new FocusArea)
    , mFocusInfo(new FocusArea)
    , mFocusResources(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mButton = panelExplore->GetButtonCancel();

    const int padding = 10;
    const int fX = panelExplore->GetX() + mButton->GetX() - padding;
    const int fY = panelExplore->GetY() + mButton->GetY() - padding;
    const int fW = mButton->GetWidth() + (padding * 2);
    const int fH = mButton->GetHeight() + (padding * 2);

    mFocusArea->SetScreenArea(fX, fY, fW, fH);
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    const int fX1 = panelInfo->GetX() + padding;
    const int fY1 = panelInfo->GetY() + padding;
    const int fW1 = panelInfo->GetWidth() - (padding * 2);
    const int fH1 = panelInfo->GetHeight() - (padding * 2);

    mFocusInfo->SetScreenArea(fX1, fY1, fW1, fH1);
    mFocusInfo->SetCornersColor(TutorialConstants::colorFocusElement);
    mFocusInfo->SetBlinking(true);
    mFocusInfo->SetVisible(false);

    const int fX2 = panelResources->GetX() + padding;
    const int fY2 = panelResources->GetY() + padding;
    const int fW2 = panelResources->GetWidth() - (padding * 2);
    const int fH2 = panelResources->GetHeight() - (padding * 2);

    mFocusResources->SetScreenArea(fX2, fY2, fW2, fH2);
    mFocusResources->SetCornersColor(TutorialConstants::colorFocusElement);
    mFocusResources->SetBlinking(true);
    mFocusResources->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(TutorialConstants::infoPlanetMapX, TutorialConstants::infoPlanetMapY);

    info->AddInfoEntry(sm->GetCString("TUT_PM_EXPLORE_TERRITORY_SUCCESS_1"), 8.f, true, true);
    info->AddActionEntry(sm->GetCString("TUT_PM_EXPLORE_TERRITORY_SUCCESS_2"), 0.f, false, false,
                         [this, fX, fY, fW, fH]
                        {
                            GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);

                            mFocusInfo->SetVisible(false);
                            mFocusResources->SetVisible(false);
                            mFocusArea->SetVisible(true);
                        });

    // EXPLORE BUTTON
    mFuncId = mButton->AddOnClickFunction([this]
    {
        SetDone();
    });
}

StepPlanetMapExploreTerritorySuccess::~StepPlanetMapExploreTerritorySuccess()
{
    delete mFocusArea;
    delete mFocusInfo;
    delete mFocusResources;

    // clear callback
    mButton->RemoveClickFunction(mFuncId);
}

void StepPlanetMapExploreTerritorySuccess::OnStart()
{
    TutorialInfoStep::OnStart();

    // FOCUS
    mFocusInfo->SetVisible(true);
    mFocusResources->SetVisible(true);
}

} // namespace game
