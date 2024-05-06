#include "Tutorial/StepPlanetMapNoInfo.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelPlanetInfo.h"
#include "Widgets/PanelPlanetResources.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>

namespace game
{

StepPlanetMapNoInfo::StepPlanetMapNoInfo(const PanelPlanetInfo * panelInfo,
                                         const PanelPlanetResources * panelResources)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // FOCUS
    const int padding = 10;

    const int fX1 = panelInfo->GetX() + padding;
    const int fY1 = panelInfo->GetY() + padding;
    const int fW1 = panelInfo->GetWidth() - (padding * 2);
    const int fH1 = panelInfo->GetHeight() - (padding * 2);

    mFocusInfo = new FocusArea;
    mFocusInfo->SetScreenArea(fX1, fY1, fW1, fH1);
    mFocusInfo->SetCornersColor(colorTutorialFocusElement);
    mFocusInfo->SetVisible(false);
    mFocusInfo->SetBlinking(true);

    const int fX2 = panelResources->GetX() + padding;
    const int fY2 = panelResources->GetY() + padding;
    const int fW2 = panelResources->GetWidth() - (padding * 2);
    const int fH2 = panelResources->GetHeight() - (padding * 2);

    mFocusResources = new FocusArea;
    mFocusResources->SetScreenArea(fX2, fY2, fW2, fH2);
    mFocusResources->SetCornersColor(colorTutorialFocusElement);
    mFocusResources->SetVisible(false);
    mFocusResources->SetBlinking(true);

    // INFO
    mInfo = new PanelInfoTutorial(infoPlanetMapW, infoPlanetMapH);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(infoPlanetMapX, infoPlanetMapY);

    mInfo->AddInfoEntry("You selected a territory, but the RESOURCES and INFO panels "
                        "do not show any data yet.", colorTutorialText, 7.f, true, true);
    mInfo->AddInfoEntry("That's because this territory is still unexplored.",
                        colorTutorialText, 5.f, true, true, [this]
                        {
                            mFocusInfo->SetVisible(false);
                            mFocusResources->SetVisible(false);
                        });

    mInfo->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

StepPlanetMapNoInfo::~StepPlanetMapNoInfo()
{
    delete mClickFilter;
    delete mFocusInfo;
    delete mFocusResources;
    delete mInfo;
}

void StepPlanetMapNoInfo::OnStart()
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
