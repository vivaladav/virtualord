#include "Tutorial/StepGameMissionGoalsIcon.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/PanelObjectActions.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>

namespace game
{

StepGameMissionGoalsIcon::StepGameMissionGoalsIcon(PanelObjectActions * panel)
    : mPanelActions(panel)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // FOCUS
    mFocusArea = new FocusArea;
    mFocusArea->SetCornersColor(colorTutorialFocusAction);
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    mInfo = new PanelInfoTutorial(590, 200);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(300, 550);

    mInfo->AddInfoEntry("For example you can check your missions goals from here.",
                        colorTutorialText, 4.f, true, false);
    mInfo->AddInfoEntry("Click this button to open the mission goals dialog.",
                        colorTutorialTextAction, 0.f, false, false, [this, panel]
                        {
                            // FOCUS
                            auto btn = panel->GetButton(PanelObjectActions::BTN_MISSION_GOALS);

                            const int padding = 10;
                            const int fX = panel->GetX() + btn->GetX() - padding;
                            const int fY = panel->GetY() + btn->GetY() - padding;
                            const int fW = btn->GetWidth() + (padding * 2);
                            const int fH = btn->GetHeight() + (padding * 2);

                            mFocusArea->SetScreenArea(fX, fY, fW, fH);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            mClickFilter->SetScreenClickableArea(fX, fY, fW, fH);
                        });

    mClickId = panel->AddButtonFunction(PanelObjectActions::BTN_MISSION_GOALS, [this]
    {
        SetDone();
    });
}

StepGameMissionGoalsIcon::~StepGameMissionGoalsIcon()
{
    mPanelActions->RemoveButtonFunction(PanelObjectActions::BTN_MISSION_GOALS, mClickId);

    delete mClickFilter;
    delete mFocusArea;
    delete mInfo;
}

void StepGameMissionGoalsIcon::OnStart()
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
