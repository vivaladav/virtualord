#include "Tutorial/StepGamePrimaryMissionGoal.h"

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

StepGamePrimaryMissionGoal::StepGamePrimaryMissionGoal(GameHUD * HUD)
    : TutorialInfoStep(900, 140)
    , mFocusArea(new FocusArea)
    , mHUD(HUD)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(510, 80);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_PRI_MGOAL_1"), 10.f, true, true);

    info->AddActionEntry(sm->GetCString("TUT_GAME_MISSION_GOALS_DIALOG_3"), 0.f, false, true,
                         [this, info]
                        {
                            auto dialog = mHUD->GetDialogMissionGoals();
                            auto btn = dialog->mPrimaryCollectButtons[0];

                            const auto cid = btn->AddOnClickFunction([info]
                                                    {
                                                        info->Continue();
                                                    });

                            mCallbacks.emplace(btn, cid);

                            const int x = btn->GetScreenX() - padding;
                            const int y = btn->GetScreenY() - padding;
                            const int w = btn->GetWidth() + (2 * padding);
                            const int h = btn->GetHeight() + (2 * padding);

                            GetClickFilter()->SetScreenClickableArea(x, y, w, h);

                            mFocusArea->SetScreenArea(x, y, w, h, true);
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);
                        });

    info->AddActionEntry(sm->GetCString("TUT_GAME_PRI_MGOAL_2"), 0.f, false, false, [this]
                        {
                            auto dialog = mHUD->GetDialogMissionGoals();
                            auto btn = dialog->GetButtonEnd();

                            const auto cid = dialog->AddFunctionOnEnd([this]
                                                     {
                                                         SetDone();
                                                     });

                            mCallbacks.emplace(btn, cid);

                            const int x = btn->GetScreenX() - padding;
                            const int y = btn->GetScreenY() - padding;
                            const int w = btn->GetWidth() + (2 * padding);
                            const int h = btn->GetHeight() + (2 * padding);

                            GetClickFilter()->SetScreenClickableArea(x, y, w, h);

                            mFocusArea->SetScreenArea(x, y, w, h, true);
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);
                        });
}

StepGamePrimaryMissionGoal::~StepGamePrimaryMissionGoal()
{
    delete mFocusArea;

    // clear callbacks
    for(auto it : mCallbacks)
        (it.first)->RemoveClickFunction(it.second);
}

void StepGamePrimaryMissionGoal::OnStart()
{
    TutorialInfoStep::OnStart();

    // move elements to front
    auto stage = sgl::sgui::Stage::Instance();
    stage->MoveChildToFront(mFocusArea);
    stage->MoveChildToFront(GetClickFilter());
    stage->MoveChildToFront(GetPanelInfo());
}

} // namespace game
