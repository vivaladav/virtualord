#include "Tutorial/StepGameWallBuildIcon.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelObjectActions.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameWallBuildIcon::StepGameWallBuildIcon(PanelObjectActions * panel)
    : TutorialInfoStep(600, 150)
    , mFocusArea(new FocusArea)
    , mPanelActions(panel)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColor(TutorialConstants::colorFocusAction);
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(1100, 650);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_BUIL_WALL_2"),
                       TutorialConstants::colorTextAction, 0.f, false, false, [this, panel]
                       {
                           // FOCUS
                           auto btn = panel->GetButton(PanelObjectActions::BTN_BUILD_WALL);

                           const int padding = 10;
                           const int fX = btn->GetScreenX() - padding;
                           const int fY = btn->GetScreenY() - padding;
                           const int fW = btn->GetWidth() + (padding * 2);
                           const int fH = btn->GetHeight() + (padding * 2);

                           mFocusArea->SetScreenArea(fX, fY, fW, fH);
                           mFocusArea->SetVisible(true);

                           // CLICK FILTER
                           GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);
                       });

    mClickId = panel->AddButtonFunction(PanelObjectActions::BTN_BUILD_WALL, [this]
    {
        SetDone();
    });
}

StepGameWallBuildIcon::~StepGameWallBuildIcon()
{
    mPanelActions->RemoveButtonFunction(PanelObjectActions::BTN_BUILD_WALL, mClickId);

    delete mFocusArea;
}

} // namespace game
