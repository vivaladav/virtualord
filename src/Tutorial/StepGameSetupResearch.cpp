#include "Tutorial/StepGameSetupResearch.h"

#include "Widgets/DialogResearch.h"
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

StepGameSetupResearch::StepGameSetupResearch(GameHUD * HUD)
    : TutorialInfoStep(600, 150)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    auto dialog = HUD->GetDialogResearch();

    // FOCUS
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(650, 100);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_RESEARCH_1"), 9.f, true, true);

    info->AddActionEntry(sm->GetCString("TUT_GAME_RESEARCH_2"), 0.f, false, true,
                        [this, dialog, info]
                        {
                            auto slider = dialog->mSliderMoney;

                            HandleSlider(slider, 100);
                        });

    info->AddActionEntry(sm->GetCString("TUT_GAME_RESEARCH_3"), 0.f, false, true,
                         [this, dialog, info]
                         {
                             auto slider = dialog->mSliderEnergy;

                             HandleSlider(slider, 50);
                         });

    info->AddActionEntry(sm->GetCString("TUT_GAME_RESEARCH_4"), 0.f, false, true,
                         [this, dialog, info]
                         {
                             auto slider = dialog->mSliderMaterial;

                             HandleSlider(slider, 50);
                         });

    info->AddActionEntry(sm->GetCString("TUT_GAME_MISSION_GOALS_DIALOG_5"), 0.f, false, false,
                         [this, dialog]
                         {
                            auto btn = dialog->mBtnClose;

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

StepGameSetupResearch::~StepGameSetupResearch()
{
    delete mFocusArea;
}

void StepGameSetupResearch::OnStart()
{
    TutorialInfoStep::OnStart();

    // move elements to front
    auto stage = sgl::sgui::Stage::Instance();
    stage->MoveChildToFront(mFocusArea);
    stage->MoveChildToFront(GetClickFilter());
    stage->MoveChildToFront(GetPanelInfo());
}

void StepGameSetupResearch::HandleSlider(sgl::sgui::Slider * slider, int target)
{
    slider->AddOnValueFinalized([this, target, slider](int val)
                                {
                                    if(val == target)
                                        GetPanelInfo()->Continue();
                                });

    // FOCUS
    const int paddingX = 20;
    const int paddingY = 20;
    const int fX = slider->GetScreenX() - paddingX;
    const int fY = slider->GetScreenY() - paddingY;
    const int fW = slider->GetWidth() + (paddingX * 2);
    const int fH = slider->GetHeight() + (paddingY * 2);

    mFocusArea->SetScreenArea(fX, fY, fW, fH, false);
    mFocusArea->SetVisible(true);

    // CLICK FILTER
    // NOTE allow big clickable area or slider won't work when setting max value outside of its body
    const int paddingX2 = 100;
    const int paddingY2 = 50;
    const int fX2 = slider->GetScreenX() - paddingX2;
    const int fY2 = slider->GetScreenY() - paddingY2;
    const int fW2 = slider->GetWidth() + (paddingX2 * 2);
    const int fH2 = slider->GetHeight() + (paddingY2 * 2);
    GetClickFilter()->SetScreenClickableArea(fX2, fY2, fW2, fH2);
}

} // namespace game
