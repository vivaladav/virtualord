#include "Tutorial/StepPlanetMapConquerTerritoryStart.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelPlanetActionConquer.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/utilities/StringManager.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepPlanetMapConquerTerritoryStart::StepPlanetMapConquerTerritoryStart(PanelPlanetActionConquer * panel)
    : TutorialInfoStep(TutorialConstants::infoPlanetMapW, TutorialConstants::infoPlanetMapH)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    mButton = panel->GetButtonOk();

    // FOCUS

    mFocusArea->SetCornersColorAction();
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(TutorialConstants::infoPlanetMapX, TutorialConstants::infoPlanetMapY);

    info->AddInfoEntry(sm->GetCString("TUT_PM_CONQUER_TERRITORY_START_1"), 0.f, false, false);

    info->SetFunctionOnFinished([this, panel]
    {
        // FOCUS
        const int padding = 10;
        const int fX = panel->GetX() + mButton->GetX() - padding;
        const int fY = panel->GetY() + mButton->GetY() - padding;
        const int fW = mButton->GetWidth() + (padding * 2);
        const int fH = mButton->GetHeight() + (padding * 2);

        mFocusArea->SetScreenArea(fX, fY, fW, fH);
        mFocusArea->SetVisible(true);

        // CLICK FILTER
        auto cf = GetClickFilter();
        cf->SetScreenClickableArea(fX, fY, fW, fH);
        cf->SetEnabled(true);
    });

    // CONQUER BUTTON
    mFuncId = mButton->AddOnClickFunction([this]
    {
        SetDone();
    });
}

StepPlanetMapConquerTerritoryStart::~StepPlanetMapConquerTerritoryStart()
{
    delete mFocusArea;

    // clear callback
    mButton->RemoveClickFunction(mFuncId);
}

} // namespace game
