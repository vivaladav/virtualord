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

StepGameTechTreeDialog::StepGameTechTreeDialog(GameHUD * HUD, TechUpgradeId upgradeID, bool showIntro)
    : TutorialInfoStep(700, 140)
    , mFocusArea(new FocusArea)
    , mHUD(HUD)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(700, 900);

    if(showIntro)
        info->AddInfoEntry(sm->GetCString("TUT_GAME_UPGRADES_1"), 9.f, true, true);

    info->AddActionEntry(sm->GetCString("TUT_GAME_UPGRADES_2"), 0.f, false, true,
                         [this, info, upgradeID]
                        {
                            auto dialog = mHUD->GetDialogTechTree();
                            mBtn = dialog->mVisibleButtonsUpgrade.at(upgradeID);

                            mCallbackBtn = mBtn->AddOnToggleFunction([info](bool checked)
                                {
                                    if(checked)
                                        info->Continue();
                                });

                            const int x = mBtn->GetScreenX() - padding;
                            const int y = mBtn->GetScreenY() - padding;
                            const int w = mBtn->GetWidth() + (2 * padding);
                            const int h = mBtn->GetHeight() + (2 * padding);

                            GetClickFilter()->SetScreenClickableArea(x, y, w, h);

                            mFocusArea->SetScreenArea(x, y, w, h, true);
                            mFocusArea->SetBlinking(true);
                            mFocusArea->SetVisible(true);
                        });

    info->AddActionEntry(sm->GetCString("TUT_GAME_UPGRADES_3"), 0.f, false, true,
                         [this, info]
                         {
                            info->SetPosition(800, 900);

                            auto dialog = mHUD->GetDialogTechTree();
                            auto btn = dialog->mBtnUnlock;

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

    info->AddActionEntry(sm->GetCString("TUT_GAME_MISSION_GOALS_DIALOG_5"), 0.f, false, false,
                         [this, info]
                        {
                            info->SetPosition(900, 20);

                            auto dialog = mHUD->GetDialogTechTree();
                            auto btn = dialog->mBtnClose;

                            const auto cid = btn->AddOnClickFunction([this]
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

StepGameTechTreeDialog::~StepGameTechTreeDialog()
{
    delete mFocusArea;

    // clear callbacks
    for(auto it : mCallbacks)
        (it.first)->RemoveClickFunction(it.second);

    mBtn->RemoveToggleFunction(mCallbackBtn);
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
