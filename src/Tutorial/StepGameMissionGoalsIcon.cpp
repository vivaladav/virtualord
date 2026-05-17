#include "Tutorial/StepGameMissionGoalsIcon.h"

#include "Widgets/PanelObjectActions.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameMissionGoalsIcon::StepGameMissionGoalsIcon(PanelObjectActions * panel, bool showIntro)
    : TutorialInfoStep(600, showIntro ? 200 : 150)
    , mFocusArea(new FocusArea)
    , mPanelActions(panel)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(200, 600);

    if(showIntro)
        info->AddInfoEntry(sm->GetCString("TUT_GAME_MISSION_GOALS_ICON_1"), 10.f, true, false);

    info->AddActionEntry(sm->GetCString("TUT_GAME_MISSION_GOALS_ICON_2"), 0.f, false, false,
                         [this, panel]
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
