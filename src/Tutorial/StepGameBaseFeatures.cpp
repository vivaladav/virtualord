#include "Tutorial/StepGameBaseFeatures.h"

#include "Widgets/PanelObjectActions.h"
#include "Widgets/PanelSelectedObject.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameBaseFeatures::StepGameBaseFeatures(const PanelSelectedObject * panelObj,
                                           PanelObjectActions * panelActions)
    : TutorialInfoStep(660, 250)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    const int padding = 10;
    const int fX = panelObj->GetX();
    const int fY = panelObj->GetY();
    const int fW = panelObj->GetWidth() + padding;
    const int fH = panelObj->GetHeight() + padding;

    mFocusArea->SetScreenArea(fX, fY, fW, fH);
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(150, 400);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_BASE_FEATURES_1"), 13.f, true, false, [this]
                       {
                           mFocusArea->SetBlinking(true);
                           mFocusArea->SetVisible(true);
                       });
    info->AddInfoEntry(sm->GetCString("TUT_GAME_BASE_FEATURES_2"), 9.f, true, false,
                       [this, panelActions, padding]
                       {
                           const int fX = panelActions->GetX() - padding;
                           const int fY = panelActions->GetY() - padding;
                           const int fW = panelActions->GetWidth() + (2 * padding);
                           const int fH = panelActions->GetHeight() + (2 * padding);

                           mFocusArea->SetBlinking(true);
                           mFocusArea->SetScreenArea(fX, fY, fW, fH);
                       });

    info->SetFunctionOnFinished([this]
    {
        SetDone();
    });
}

StepGameBaseFeatures::~StepGameBaseFeatures()
{
    delete mFocusArea;
}

} // namespace game
