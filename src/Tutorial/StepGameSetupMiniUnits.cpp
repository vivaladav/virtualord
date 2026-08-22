#include "Tutorial/StepGameSetupMiniUnits.h"

#include "Widgets/DialogNewMiniUnitsSquad.h"
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

StepGameSetupMiniUnits::StepGameSetupMiniUnits(GameHUD * HUD, const std::vector<int> & values,
                                               GameObjectTypeId type)
    : TutorialInfoStep(600, 150)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    auto dialog = HUD->GetDialogNewMiniUnitsSquad();

    // FOCUS
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(650, 70);

    // find and set type if needed
    if(dialog->GetTypeToBuild() != type)
    {
        info->AddActionEntry(sm->GetCString("TUT_GAME_BUILD_MU_5"), 0.f, false, true,
                            [this, dialog, type]
                            {
                                auto btn = dialog->mBtnRight;

                                // NOTE no need to remove the function later as the dialog is
                                // destroyed when the button is clicked
                                btn->AddOnClickFunction([this, dialog, type]
                                                    {
                                                        if(dialog->GetTypeToBuild() == type)
                                                            GetPanelInfo()->Continue();
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

    // set number of elements, if needed
    if(dialog->mSliderElements->GetValue() != values[0])
    {
        const std::string str1 = sm->GetParametricString("TUT_GAME_BUILD_MU_1",
                                                         std::to_string(values[0]));

        info->AddActionEntry(str1.c_str(), 0.f, false, true,
                            [this, dialog, info, values]
                            {
                                auto slider = dialog->mSliderElements;

                                HandleSlider(slider, values[0]);
                            });
    }

    // set number of squads, if needed
    if(dialog->mSliderSquads->GetValue() != values[1])
    {
        const std::string str2 = sm->GetParametricString("TUT_GAME_BUILD_MU_2",
                                                         std::to_string(values[1]));

        info->AddActionEntry(str2.c_str(), 0.f, false, true,
                             [this, dialog, info, values]
                             {
                                 auto slider = dialog->mSliderSquads;

                                 HandleSlider(slider, values[1]);
                             });
    }

    info->AddActionEntry(sm->GetCString("TUT_GAME_BUILD_MU_3"), 0.f, false, true,
                         [this, dialog]
                         {
                            auto btn = dialog->mBtnBuild;

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

StepGameSetupMiniUnits::~StepGameSetupMiniUnits()
{
    delete mFocusArea;
}

void StepGameSetupMiniUnits::OnStart()
{
    TutorialInfoStep::OnStart();

    // move elements to front
    auto stage = sgl::sgui::Stage::Instance();
    stage->MoveChildToFront(mFocusArea);
    stage->MoveChildToFront(GetClickFilter());
    stage->MoveChildToFront(GetPanelInfo());
}

void StepGameSetupMiniUnits::HandleSlider(sgl::sgui::Slider * slider, int target)
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
