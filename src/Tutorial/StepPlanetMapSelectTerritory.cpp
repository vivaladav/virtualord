#include "Tutorial/StepPlanetMapSelectTerritory.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PlanetMap.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>

namespace game
{

StepPlanetMapSelectTerritory::StepPlanetMapSelectTerritory(PlanetMap * planet)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // FOCUS
    mTerritory = planet->GetButton(0);

    const int padding = 10;
    const int fX = planet->GetX() + mTerritory->GetX() - padding;
    const int fY = planet->GetY() + mTerritory->GetY() - padding;
    const int fW = mTerritory->GetWidth() + (padding * 2);
    const int fH = mTerritory->GetHeight() + (padding * 2);

    mFocusArea = new FocusArea;
    mFocusArea->SetScreenArea(fX, fY, fW, fH);
    mFocusArea->SetCornersColor(colorTutorialFocusElement);
    mFocusArea->SetVisible(false);

    // INFO
    mInfo = new PanelInfoTutorial(infoPlanetMapW, infoPlanetMapH);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(infoPlanetMapX, infoPlanetMapY);

    mInfo->AddInfoEntry("Let's start to explore a territory of this planet.",
                        colorTutorialText, 5.f, true, true);
    mInfo->AddInfoEntry("Select this one in the top with the LEFT MOUSE BUTTON", colorTutorialTextAction,
                        0.f, false, false);

    mInfo->SetFunctionOnFinished([this, fX, fY, fW, fH]
    {
        mFocusArea->SetCornersColor(colorTutorialFocusAction);
        mFocusArea->SetBlinking(true);

        mClickFilter->SetScreenClickableArea(fX, fY, fW, fH);

        mCheckTerritorySelected = true;
    });
}

StepPlanetMapSelectTerritory::~StepPlanetMapSelectTerritory()
{
    delete mClickFilter;
    delete mFocusArea;
    delete mInfo;
}

void StepPlanetMapSelectTerritory::OnStart()
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

void StepPlanetMapSelectTerritory::Update(float)
{
    if(mCheckTerritorySelected)
    {
        if(mTerritory->IsChecked())
            SetDone();
    }
}

} // namespace game
