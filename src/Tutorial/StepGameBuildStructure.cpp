#include "Tutorial/StepGameBuildStructure.h"

#include "Widgets/DialogNewElement.h"
#include "Widgets/GameHUD.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/ButtonsGroup.h>
#include <sgl/sgui/Stage.h>
#include <sgl/utilities/StringManager.h>

namespace
{
constexpr int padding = 10;
}

namespace game
{

StepGameBuildStructure::StepGameBuildStructure(GameHUD * HUD, const char * textCat,
                                               const char * textStruct, int indCat, int indStruct)
    : TutorialInfoStep(550, 130)
    , mFocusArea(new FocusArea)
    , mHUD(HUD)
    , mIndCat(indCat)
    , mIndStruct(indStruct)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(550, 870);

    if(textCat != nullptr)
    {
        info->AddActionEntry(sm->GetCString(textCat), 0.f, false, true,
                             [this, HUD, info]
                            {
                                auto dialog = HUD->GetDialogNewElement();
                                auto btn = dialog->mButtonsStructures->GetButton(mIndCat);

                                mButtonCatId = btn->AddOnToggleFunction([info](bool checked)
                                                                        {
                                                                            if(checked)
                                                                                info->Continue();
                                                                        });

                                // CLICK FILTER
                                const int fX = btn->GetScreenX();
                                const int fY = btn->GetScreenY();
                                const int fW = btn->GetWidth();
                                const int fH = btn->GetHeight();

                                GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);

                                // FOCUS
                                const int fX2 = fX - padding;
                                const int fY2 = fY - padding;
                                const int fW2 = fW + (padding * 2);
                                const int fH2 = fH + (padding * 2);

                                mFocusArea->SetScreenArea(fX2, fY2, fW2, fH2, true);
                                mFocusArea->SetVisible(true);
                            });
    }

    if(textStruct != nullptr)
    {
        info->AddActionEntry(sm->GetCString(textStruct), 0.f, false, true,
                             [this, HUD, info]
                            {
                                auto dialog = HUD->GetDialogNewElement();
                                auto btn = dialog->mSlots->GetButton(mIndStruct);

                                mButtonStructId = btn->AddOnToggleFunction([info](bool checked)
                                                                           {
                                                                               if(checked)
                                                                                   info->Continue();
                                                                           });

                                // CLICK FILTER
                                const int fX = btn->GetScreenX();
                                const int fY = btn->GetScreenY();
                                const int fW = btn->GetWidth();
                                const int fH = btn->GetHeight();

                                GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);

                                // FOCUS
                                const int fX2 = fX - padding;
                                const int fY2 = fY - padding;
                                const int fW2 = fW + (padding * 2);
                                const int fH2 = fH + (padding * 2);

                                mFocusArea->SetScreenArea(fX2, fY2, fW2, fH2, true);
                                mFocusArea->SetVisible(true);
                           });
    }

    info->AddActionEntry(sm->GetCString("TUT_GAME_BASE_BUILD_UNIT_3"), 0.f, false, true,
                         [this, HUD]
                        {
                            auto dialog = HUD->GetDialogNewElement();
                            auto btn = dialog->mBtnBuild;

                            mButtonBuildId = btn->AddOnClickFunction([this]
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

StepGameBuildStructure::~StepGameBuildStructure()
{
    delete mFocusArea;

    // remove toggle functions
    auto dialog = mHUD->GetDialogNewElement();

    if(nullptr == dialog)
        return ;

    if(mButtonCatId != 0)
    {
        auto btn = dialog->mButtonsStructures->GetButton(mIndCat);
        btn->RemoveToggleFunction(mButtonCatId);
    }

    if(mButtonStructId != 0)
    {
        auto btn = dialog->mSlots->GetButton(mIndStruct);
        btn->RemoveToggleFunction(mButtonStructId);
    }

    if(mButtonBuildId != 0)
    {
        auto btn = dialog->mBtnBuild;
        btn->RemoveClickFunction(mButtonBuildId);
    }
}

void StepGameBuildStructure::OnStart()
{
    TutorialInfoStep::OnStart();

    // move elements to front
    auto stage = sgl::sgui::Stage::Instance();
    stage->MoveChildToFront(mFocusArea);
    stage->MoveChildToFront(GetClickFilter());
    stage->MoveChildToFront(GetPanelInfo());
}

} // namespace game
