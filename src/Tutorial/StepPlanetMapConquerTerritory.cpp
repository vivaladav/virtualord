#include "Tutorial/StepPlanetMapConquerTerritory.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelPlanetActions.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>

namespace game
{

StepPlanetMapConquerTerritory::StepPlanetMapConquerTerritory(PanelPlanetActions * panelActions)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // FOCUS
    auto btn = panelActions->GetButton(PanelPlanetActions::CONQUER);

    const int padding = 10;
    const int fX = panelActions->GetX() + btn->GetX() - padding;
    const int fY = panelActions->GetY() + btn->GetY() - padding;
    const int fW = btn->GetWidth() + (padding * 2);
    const int fH = btn->GetHeight() + (padding * 2);

    mFocusArea = new FocusArea;
    mFocusArea->SetScreenArea(fX, fY, fW, fH);
    mFocusArea->SetCornersColor(colorTutorialFocusAction);
    mFocusArea->SetVisible(false);

    // INFO
    mInfo = new PanelInfoTutorial(infoPlanetMapW, infoPlanetMapH);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(infoPlanetMapX, infoPlanetMapY);

    mInfo->AddInfoEntry("I would suggest you to conquer the territory yourself.",
                        colorTutorialText, 5.f, true, true);
    mInfo->AddInfoEntry("To do that click the button CONQUER.",
                        colorTutorialTextAction, 0.f, false, false, [this, fX, fY, fW, fH]
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

StepPlanetMapConquerTerritory::~StepPlanetMapConquerTerritory()
{
    delete mClickFilter;
    delete mFocusArea;
    delete mInfo;
}

void StepPlanetMapConquerTerritory::OnStart()
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
