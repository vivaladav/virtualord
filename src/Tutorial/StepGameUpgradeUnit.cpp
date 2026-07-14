#include "Tutorial/StepGameUpgradeUnit.h"

#include "GameConstants.h"
#include "Widgets/DialogUpgrade.h"
#include "Widgets/GameHUD.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/Stage.h>
#include <sgl/utilities/StringManager.h>

namespace
{
const int padding = 10;
}

namespace game
{

StepGameUpgradeUnit::StepGameUpgradeUnit(GameHUD * HUD, bool showIntro)
    : TutorialInfoStep(670, showIntro ? 230 : 175)
    , mFocusArea(new FocusArea)
    , mHUD(HUD)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(450, 820);

    if(showIntro)
        info->AddInfoEntry(sm->GetCString("TUT_GAME_UPGRADE_3"), 10.f, true, false);

    info->AddActionEntry(sm->GetCString("TUT_GAME_UPGRADE_4"), 0.f, false, false, [this, HUD, info]
                        {
                            auto dialog = HUD->GetDialogUpgrade();
                            auto btn = dialog->mButtonsInc[OBJ_ATT_ENERGY];

                            mButtonIncId = btn->AddOnClickFunction([info]
                                                                   {
                                                                       info->Continue();
                                                                   });

                            // FOCUS
                            const int fX = btn->GetScreenX() - padding;
                            const int fY = btn->GetScreenY() - padding;
                            const int fW = btn->GetWidth() + (padding * 2);
                            const int fH = btn->GetHeight() + (padding * 2);

                            mFocusArea->SetScreenArea(fX, fY, fW, fH, true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);
                        });
    info->AddActionEntry(sm->GetCString("TUT_GAME_UPGRADE_5"), 0.f, false, false, [this, HUD]
                       {
                            auto dialog = HUD->GetDialogUpgrade();
                            auto btn = dialog->mBtnUpgrade;

                            mButtonUpgId = btn->AddOnClickFunction([this]
                                                                   {
                                                                       SetDone();
                                                                   });

                            // FOCUS
                            const int fX = btn->GetScreenX() - padding;
                            const int fY = btn->GetScreenY() - padding;
                            const int fW = btn->GetWidth() + (padding * 2);
                            const int fH = btn->GetHeight() + (padding * 2);

                            mFocusArea->SetScreenArea(fX, fY, fW, fH, true);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);
                       });
}

StepGameUpgradeUnit::~StepGameUpgradeUnit()
{
    delete mFocusArea;
}

void StepGameUpgradeUnit::OnStart()
{
    TutorialInfoStep::OnStart();

    // move elements to front
    auto stage = sgl::sgui::Stage::Instance();
    stage->MoveChildToFront(mFocusArea);
    stage->MoveChildToFront(GetClickFilter());
    stage->MoveChildToFront(GetPanelInfo());
}

void StepGameUpgradeUnit::OnEnd()
{
    auto dialog = mHUD->GetDialogUpgrade();

    if(nullptr == dialog)
        return ;

    auto btn = dialog->mButtonsInc[OBJ_ATT_ENERGY];
    btn->RemoveClickFunction(mButtonIncId);

    btn = dialog->mBtnUpgrade;
    btn->RemoveClickFunction(mButtonUpgId);
}

} // namespace game
