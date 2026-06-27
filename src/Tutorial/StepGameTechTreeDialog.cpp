#include "Tutorial/StepGameTechTreeDialog.h"

#include "Widgets/ButtonTechUpgrade.h"
#include "Widgets/DialogTechTree.h"
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

StepGameTechTreeDialog::StepGameTechTreeDialog(GameHUD * HUD)
    : TutorialInfoStep(700, 140)
    , mFocusArea(new FocusArea)
    , mHUD(HUD)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColorElement();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(700, 900);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_UPGRADES_1"), 9.f, true, true);

    info->AddActionEntry(sm->GetCString("TUT_GAME_UPGRADES_2"), 0.f, false, true,
                         [this, info]
                        {
                            info->SetPosition(300, 900);

                            auto dialog = mHUD->GetDialogTechTree();
                            auto btn = dialog->mVisibleButtonsUpgrade[0];

                            // NOTE no need to remove the function later as the dialog is
                            // destroyed at the end
                            btn->AddOnToggleFunction([info](bool checked)
                                {
                                    if(checked)
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

    info->AddActionEntry(sm->GetCString("TUT_GAME_UPGRADES_3"), 0.f, false, true,
                         [this, info]
                         {
                            info->SetPosition(800, 900);

                            auto dialog = mHUD->GetDialogTechTree();
                            auto btn = dialog->mBtnUnlock;

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

    info->AddActionEntry(sm->GetCString("TUT_GAME_MISSION_GOALS_DIALOG_5"), 0.f, false, false,
                         [this, info]
                        {
                            info->SetPosition(900, 20);

                            auto dialog = mHUD->GetDialogTechTree();
                            auto btn = dialog->mBtnClose;

                            // NOTE no need to remove the function later as the dialog is
                            // destroyed when the button is clicked
                            btn->AddOnClickFunction([this]
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

StepGameTechTreeDialog::~StepGameTechTreeDialog()
{
    delete mFocusArea;
}

void StepGameTechTreeDialog::OnStart()
{
    TutorialInfoStep::OnStart();

    // move elements to front
    auto stage = sgl::sgui::Stage::Instance();
    stage->MoveChildToFront(mFocusArea);
    stage->MoveChildToFront(GetClickFilter());
    stage->MoveChildToFront(GetPanelInfo());
}

} // namespace game
