#include "Tutorial/StepPlanetMapSelectTerritory.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PlanetMap.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepPlanetMapSelectTerritory::StepPlanetMapSelectTerritory(PlanetMap * planet, unsigned int missionInd,
                                                           bool intro)
    : TutorialInfoStep(TutorialConstants::infoPlanetMapW, TutorialConstants::infoPlanetMapH)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mTerritory = planet->GetButton(missionInd);

    const int padding = 10;
    const int fX = planet->GetX() + mTerritory->GetX() - padding;
    const int fY = planet->GetY() + mTerritory->GetY() - padding;
    const int fW = mTerritory->GetWidth() + (padding * 2);
    const int fH = mTerritory->GetHeight() + (padding * 2);

    mFocusArea->SetScreenArea(fX, fY, fW, fH, false);
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(TutorialConstants::infoPlanetMapX, TutorialConstants::infoPlanetMapY);

    if(intro)
        info->AddInfoEntry(sm->GetCString("TUT_PM_SELECT_TERRITORY_1"), 6.f, true, true);

    info->AddActionEntry(sm->GetCString("TUT_PM_SELECT_TERRITORY_2"), 0.f, false, false);

    info->SetFunctionOnFinished([this, fX, fY, fW, fH]
    {
        mFocusArea->SetCornersColorAction();
        mFocusArea->SetBlinking(true);

        GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);

        mCheckTerritorySelected = true;
    });
}

StepPlanetMapSelectTerritory::~StepPlanetMapSelectTerritory()
{
    delete mFocusArea;
}

void StepPlanetMapSelectTerritory::OnStart()
{
    TutorialInfoStep::OnStart();

    // FOCUS
    mFocusArea->SetVisible(true);
}

void StepPlanetMapSelectTerritory::Update(float)
{
    if(mCheckTerritorySelected)
    {
        if(mTerritory->IsChecked())
            SetDone();
    }
}

} // namespace game
