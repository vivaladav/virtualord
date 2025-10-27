#include "Tutorial/StepGameMissionGoalsDialog.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/DialogMissionGoals.h"
#include "Widgets/GameHUD.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/Stage.h>

namespace game
{

StepGameMissionGoalsDialog::StepGameMissionGoalsDialog(GameHUD * HUD)
    : mHUD(HUD)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // FOCUS
    mFocusArea = new FocusArea;
    mFocusArea->SetCornersColor(colorTutorialFocusElement);
    mFocusArea->SetVisible(false);

    // INFO
    mInfo = new PanelInfoTutorial(900, 130);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(510, 65);

    mInfo->AddInfoEntry("In this dialog you can find your primary goals, which are the ones you need "
                        "to complete in order to win this mission.",
                        colorTutorialText, 12.f, true, true, [this]
                        {
                            mFocusArea->SetScreenArea(340, 290, 1240, 160);
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);
                        });

    mInfo->AddInfoEntry("Below there are your secondary goals, which you can complete to gain extra rewards.",
                        colorTutorialText, 8.f, true, true, [this]
                        {
                            mFocusArea->SetScreenArea(340, 460, 1240, 345);
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);
                        });

    mInfo->AddInfoEntry("Here there are the COLLECT buttons that will allow you to get your rewards once "
                        "you complete a mission goal.",
                        colorTutorialText, 12.f, true, true, [this]
                        {
                            mFocusArea->SetScreenArea(1300, 340, 280, 440);
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);
                        });

    mInfo->AddInfoEntry("Finally there's the END MISSION button that will be enabled once you complete "
                        "all your primary goals.",
                        colorTutorialText, 12.f, true, true, [this]
                        {
                            const int padding = 10;
                            auto btn = mHUD->GetDialogMissionGoals()->GetButtonEnd();

                            const int x = btn->GetScreenX() - padding;
                            const int y = btn->GetScreenY() - padding;
                            const int w = btn->GetWidth() + (2 * padding);
                            const int h = btn->GetHeight() + (2 * padding);

                            mFocusArea->SetScreenArea(x, y, w, h);
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);
                        });

    mInfo->AddInfoEntry("Ok commander, let's move on!\n", colorTutorialText, 6.f, true, true);

    mInfo->AddInfoEntry("Close the dialog clicking on the button with the X on the top-right corner.",
                        colorTutorialTextAction, 0.f, false, false, [this]
                        {
                            const int padding = 10;

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

                            mClickFilter->SetScreenClickableArea(x, y, w, h);

                            mFocusArea->SetScreenArea(x, y, w, h);
                            mFocusArea->SetCornersColor(colorTutorialFocusAction);
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);
                        });
}

StepGameMissionGoalsDialog::~StepGameMissionGoalsDialog()
{
    delete mClickFilter;
    delete mFocusArea;
    delete mInfo;
}

void StepGameMissionGoalsDialog::OnStart()
{
    // CLICK FILTER
    mClickFilter->SetEnabled(true);

    // INFO
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    // move focus to front
    sgl::sgui::Stage::Instance()->MoveChildToFront(mFocusArea);

    mInfo->StartInfo();
}

} // namespace game
