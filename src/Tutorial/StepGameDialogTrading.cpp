#include "Tutorial/StepGameDialogTrading.h"

#include "Widgets/DialogTrading.h"
#include "Widgets/GameHUD.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/Slider.h>
#include <sgl/sgui/Stage.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameDialogTrading::StepGameDialogTrading(GameHUD * HUD)
    : TutorialInfoStep(600, 150)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    auto dialog = HUD->GetDialogTrading();

    // FOCUS
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(650, 100);

    info->AddActionEntry(sm->GetCString("TUT_GAME_TRAD_POST_1"), 0.f, false, true,
                        [this, dialog, info]
                        {
                            HandleButton(dialog->mButtonsSellPlus[DialogTrading::TR_DIAMONDS]);
                        });

    info->AddActionEntry(sm->GetCString("TUT_GAME_TRAD_POST_1"), 0.f, false, true,
                         [this, dialog, info]
                         {
                            HandleButton(dialog->mButtonsSellPlus[DialogTrading::TR_BLOBS]);
                         });

    info->AddActionEntry(sm->GetCString("TUT_GAME_TRAD_POST_1"), 0.f, false, true,
                         [this, dialog, info]
                         {
                            HandleButton(dialog->mButtonSell);
                         });

    info->AddActionEntry(sm->GetCString("TUT_GAME_MISSION_GOALS_DIALOG_5"), 0.f, false, false,
                         [this, dialog]
                         {
                            auto btn = dialog->mButtonClose;

                             // NOTE no need to remove the function later as the dialog is
                             // destroyed when the button is clicked
                             btn->AddOnClickFunction([this]
                                                    {
                                                        SetDone();
                                                    });

                             const int padding = 10;
                             const int x = btn->GetScreenX() - padding;
                             const int y = btn->GetScreenY() - padding;
                             const int w = btn->GetWidth() + (2 * padding);
                             const int h = btn->GetHeight() + (2 * padding);

                             GetClickFilter()->SetScreenClickableArea(x, y, w, h);

                             mFocusArea->SetScreenArea(x, y, w, h, true);
                             mFocusArea->SetCornersColorAction();
                             mFocusArea->SetBlinking(true);
                             mFocusArea->SetVisible(true);
                         });
}

StepGameDialogTrading::~StepGameDialogTrading()
{
    delete mFocusArea;
}

void StepGameDialogTrading::OnStart()
{
    TutorialInfoStep::OnStart();

    // move elements to front
    auto stage = sgl::sgui::Stage::Instance();
    stage->MoveChildToFront(mFocusArea);
    stage->MoveChildToFront(GetClickFilter());
    stage->MoveChildToFront(GetPanelInfo());
}

void StepGameDialogTrading::HandleButton(sgl::sgui::AbstractButton * btn)
{
    auto info = GetPanelInfo();

    btn->AddOnClickFunction([info]
                            {
                                info->Continue();
                            });

    // FOCUS
    const int padding = 10;
    const int fX = btn->GetScreenX() - padding;
    const int fY = btn->GetScreenY() - padding;
    const int fW = btn->GetWidth() + (padding * 2);
    const int fH = btn->GetHeight() + (padding * 2);

    mFocusArea->SetScreenArea(fX, fY, fW, fH, true);
    mFocusArea->SetVisible(true);

    // CLICK FILTER
    const int fX2 = btn->GetScreenX();
    const int fY2 = btn->GetScreenY();
    const int fW2 = btn->GetWidth();
    const int fH2 = btn->GetHeight();
    GetClickFilter()->SetScreenClickableArea(fX2, fY2, fW2, fH2);
}

} // namespace game
