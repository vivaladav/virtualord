#include "Tutorial/StepGameUpgradeUnitFree.h"

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

StepGameUpgradeUnitFree::StepGameUpgradeUnitFree(GameHUD * HUD)
    : TutorialInfoStep(670, 175)
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

    info->AddActionEntry(sm->GetCString("TUT_GAME_UPGRADE_4b"), 0.f, false, false, [this, HUD, info]
                        {
                            auto dialog = HUD->GetDialogUpgrade();

                            const unsigned int numButtons = dialog->mButtonsInc.size();
                            auto btn0 = dialog->mButtonsInc[OBJ_ATT_ENERGY];
                            auto btn1 = dialog->mButtonsInc[numButtons - 1];

                            for(auto btn : dialog->mButtonsInc)
                            {
                                btn->AddOnClickFunction([info]
                                                        {
                                                            info->Continue();
                                                        });
                            }

                            // FOCUS
                            const int fX = btn0->GetScreenX() - padding;
                            const int fY = btn0->GetScreenY() - padding;
                            const int fW = btn0->GetWidth() + (padding * 2);
                            const int fH = (btn1->GetScreenY() - fY) + btn1->GetHeight() +
                                           (padding * 2);

                            mFocusArea->SetScreenArea(fX, fY, fW, fH);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);
                        });
    info->AddActionEntry(sm->GetCString("TUT_GAME_UPGRADE_5"), 0.f, false, false, [this, HUD]
                       {
                            auto dialog = HUD->GetDialogUpgrade();
                            auto btn = dialog->mBtnUpgrade;

                            btn->AddOnClickFunction([this]
                                                    {
                                                        SetDone();
                                                    });

                            // FOCUS
                            const int fX = btn->GetScreenX() - padding;
                            const int fY = btn->GetScreenY() - padding;
                            const int fW = btn->GetWidth() + (padding * 2);
                            const int fH = btn->GetHeight() + (padding * 2);

                            mFocusArea->SetScreenArea(fX, fY, fW, fH);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);
                       });
}

StepGameUpgradeUnitFree::~StepGameUpgradeUnitFree()
{
    delete mFocusArea;
}

void StepGameUpgradeUnitFree::OnStart()
{
    TutorialInfoStep::OnStart();

    // move elements to front
    auto stage = sgl::sgui::Stage::Instance();
    stage->MoveChildToFront(mFocusArea);
    stage->MoveChildToFront(GetClickFilter());
    stage->MoveChildToFront(GetPanelInfo());
}

} // namespace game
