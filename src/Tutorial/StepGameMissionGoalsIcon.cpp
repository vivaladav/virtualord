#include "Tutorial/StepGameMissionGoalsIcon.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelObjectActions.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameMissionGoalsIcon::StepGameMissionGoalsIcon(PanelObjectActions * panel)
    : TutorialInfoStep(600, 200)
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

    info->SetPosition(290, 550);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_MISSION_GOALS_ICON_1"),
                       TutorialConstants::colorText, 5.f, true, false);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_MISSION_GOALS_ICON_2"),
                       TutorialConstants::colorTextAction, 0.f, false, false, [this, panel]
                       {
                           // FOCUS
                           auto btn = panel->GetButton(PanelObjectActions::BTN_MISSION_GOALS);

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

    mClickId = panel->AddButtonFunction(PanelObjectActions::BTN_MISSION_GOALS, [this]
    {
        SetDone();
    });
}

StepGameMissionGoalsIcon::~StepGameMissionGoalsIcon()
{
    mPanelActions->RemoveButtonFunction(PanelObjectActions::BTN_MISSION_GOALS, mClickId);

    delete mFocusArea;
}

} // namespace game
