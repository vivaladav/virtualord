#include "Tutorial/StepPlanetMapExploreTerritory.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelPlanetActions.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>

namespace game
{

StepPlanetMapExploreTerritory::StepPlanetMapExploreTerritory(PanelPlanetActions * panelActions)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // FOCUS
    auto btnExplore = panelActions->GetButton(PanelPlanetActions::EXPLORE);

    const int padding = 10;
    const int fX = panelActions->GetX() + btnExplore->GetX() - padding;
    const int fY = panelActions->GetY() + btnExplore->GetY() - padding;
    const int fW = btnExplore->GetWidth() + (padding * 2);
    const int fH = btnExplore->GetHeight() + (padding * 2);

    mFocusArea = new FocusArea;
    mFocusArea->SetScreenArea(fX, fY, fW, fH);
    mFocusArea->SetCornersColor(colorTutorialFocusAction);
    mFocusArea->SetVisible(false);

    // INFO
    mInfo = new PanelInfoTutorial(infoPlanetMapW, infoPlanetMapH);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(infoPlanetMapX, infoPlanetMapY);

    mInfo->AddInfoEntry("Click the button EXPLORE to try to explore it.",
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

StepPlanetMapExploreTerritory::~StepPlanetMapExploreTerritory()
{
    delete mClickFilter;
    delete mFocusArea;
    delete mInfo;
}

void StepPlanetMapExploreTerritory::OnStart()
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
