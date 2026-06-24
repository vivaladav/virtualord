#include "Tutorial/StepGameSecondaryMissionGoal.h"

#include "Widgets/DialogMissionGoals.h"
#include "Widgets/GameHUD.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/Stage.h>
#include <sgl/utilities/StringManager.h>

namespace
{
constexpr int padding = 10;
}

namespace game
{

StepGameSecondaryMissionGoal::StepGameSecondaryMissionGoal(GameHUD * HUD, int goal)
    : TutorialInfoStep(800, 140)
    , mFocusArea(new FocusArea)
    , mHUD(HUD)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(500, 80);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_SEC_MGOAL_1"), 9.f, true, true);

    info->AddActionEntry(sm->GetCString("TUT_GAME_MISSION_GOALS_DIALOG_3"), 0.f, false, true,
                         [this, info, goal]
                        {
                            auto dialog = mHUD->GetDialogMissionGoals();
                            auto btn = dialog->mSecondCollectButtons[goal];

                            // NOTE no need to remove the function later as the dialog is
                            // destroyed at the end
                            btn->AddOnClickFunction([info]
                                                    {
                                                        info->Continue();
                                                    });

                            const int x = btn->GetScreenX() - padding;
                            const int y = btn->GetScreenY() - padding;
                            const int w = btn->GetWidth() + (2 * padding);
                            const int h = btn->GetHeight() + (2 * padding);

                            GetClickFilter()->SetScreenClickableArea(x, y, w, h);

                            mFocusArea->SetScreenArea(x, y, w, h);
                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);
                        });

    info->AddActionEntry(sm->GetCString("TUT_GAME_MISSION_GOALS_DIALOG_5"), 0.f, false, false, [this]
                        {
                            auto dialog = mHUD->GetDialogMissionGoals();
                            auto btn = dialog->GetButtonClose();

                            // NOTE no need to remove the function later as the dialog is
                            // destroyed when the button is clicked
                            dialog->AddFunctionOnClose([this]
                                                       {
                                                           SetDone();
                                                       });

                            const int x = btn->GetScreenX() - padding;
                            const int y = btn->GetScreenY() - padding;
                            const int w = btn->GetWidth() + (2 * padding);
                            const int h = btn->GetHeight() + (2 * padding);

                            GetClickFilter()->SetScreenClickableArea(x, y, w, h);

                            mFocusArea->SetScreenArea(x, y, w, h);
                            mFocusArea->SetCornersColorAction();
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);
                        });
}

StepGameSecondaryMissionGoal::~StepGameSecondaryMissionGoal()
{
    delete mFocusArea;
}

void StepGameSecondaryMissionGoal::OnStart()
{
    TutorialInfoStep::OnStart();

    // move elements to front
    auto stage = sgl::sgui::Stage::Instance();
    stage->MoveChildToFront(mFocusArea);
    stage->MoveChildToFront(GetClickFilter());
    stage->MoveChildToFront(GetPanelInfo());
}

} // namespace game
