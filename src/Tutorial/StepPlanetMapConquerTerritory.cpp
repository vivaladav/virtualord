#include "Tutorial/StepPlanetMapConquerTerritory.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelPlanetActions.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepPlanetMapConquerTerritory::StepPlanetMapConquerTerritory(PanelPlanetActions * panelActions)
    : TutorialInfoStep(TutorialConstants::infoPlanetMapW, TutorialConstants::infoPlanetMapH)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    mButton = panelActions->GetButton(PanelPlanetActions::CONQUER);

    // FOCUS
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(TutorialConstants::infoPlanetMapX, TutorialConstants::infoPlanetMapY);

    info->AddInfoEntry(sm->GetCString("TUT_PM_CONQUER_TERRITORY_1"), 6.f, true, true);
    info->AddActionEntry(sm->GetCString("TUT_PM_CONQUER_TERRITORY_2"), 0.f, false, false,
                         [this, panelActions]
                        {
                            // FOCUS
                            const int padding = 5;
                            const int fX = panelActions->GetX() + mButton->GetX() - padding;
                            const int fY = panelActions->GetY() + mButton->GetY() - padding;
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

StepPlanetMapConquerTerritory::~StepPlanetMapConquerTerritory()
{
    delete mFocusArea;

    // clear callback
    mButton->RemoveClickFunction(mFuncId);
}

} // namespace game
