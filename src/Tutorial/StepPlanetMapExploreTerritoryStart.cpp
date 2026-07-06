#include "Tutorial/StepPlanetMapExploreTerritoryStart.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelPlanetActionExplore.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepPlanetMapExploreTerritoryStart::StepPlanetMapExploreTerritoryStart(PanelPlanetActionExplore * panelExplore)
    : TutorialInfoStep(TutorialConstants::infoPlanetMapW, TutorialConstants::infoPlanetMapH)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mButton = panelExplore->GetButtonOk();

    const int padding = 10;
    const int fX = panelExplore->GetX() + mButton->GetX() - padding;
    const int fY = panelExplore->GetY() + mButton->GetY() - padding;
    const int fW = mButton->GetWidth() + (padding * 2);
    const int fH = mButton->GetHeight() + (padding * 2);

    mFocusArea->SetScreenArea(fX, fY, fW, fH);
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(TutorialConstants::infoPlanetMapX, TutorialConstants::infoPlanetMapY);

    info->AddActionEntry(sm->GetCString("TUT_PM_EXPLORE_TERRITORY_START_1"), 0.f, false, false,
                         [this, fX, fY, fW, fH]
                        {
                            GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);
                        });

    // EXPLORE BUTTON
    mFuncId = mButton->AddOnClickFunction([this]
    {
        SetDone();
    });
}

StepPlanetMapExploreTerritoryStart::~StepPlanetMapExploreTerritoryStart()
{
    delete mFocusArea;

    // clear callback
    mButton->RemoveClickFunction(mFuncId);
}

void StepPlanetMapExploreTerritoryStart::OnStart()
{
    TutorialInfoStep::OnStart();

    // FOCUS
    mFocusArea->SetVisible(true);
}

} // namespace game
