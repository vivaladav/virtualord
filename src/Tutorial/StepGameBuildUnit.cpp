#include "Tutorial/StepGameBuildUnit.h"

#include "GameObjects/Unit.h"
#include "Tutorial/TutorialConstants.h"
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

StepGameBuildUnit::StepGameBuildUnit(GameHUD * HUD, const char * textUnit, const char * textBuild,
                                     int unitInd)
    : TutorialInfoStep(550, 130)
    , mFocusArea(new FocusArea)
    , mHUD(HUD)
    , mIndStruct(unitInd)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(550, 870);

    if(textUnit != nullptr)
    {
        info->AddActionEntry(sm->GetCString(textUnit), 0.f, false, true,
                             [this, HUD, info]
                            {
                                auto dialog = HUD->GetDialogNewElement();
                                auto btn = dialog->mSlots->GetButton(mIndStruct);

                                mButtonUnitId = btn->AddOnToggleFunction([info](bool checked)
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

                                mFocusArea->SetScreenArea(fX2, fY2, fW2, fH2, true, mDelayAnim);
                                mFocusArea->SetVisible(true);
                           });
    }

    info->AddActionEntry(sm->GetCString(textBuild), 0.f, false, true,
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

                            mFocusArea->SetScreenArea(fX, fY, fW, fH, true, mDelayAnim);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);
                        });
}

StepGameBuildUnit::~StepGameBuildUnit()
{
    delete mFocusArea;

    // remove toggle functions
    auto dialog = mHUD->GetDialogNewElement();

    if(nullptr == dialog)
        return ;

    if(mButtonUnitId != 0)
    {
        auto btn = dialog->mSlots->GetButton(mIndStruct);
        btn->RemoveToggleFunction(mButtonUnitId);
    }

    if(mButtonBuildId != 0)
    {
        auto btn = dialog->mBtnBuild;
        btn->RemoveClickFunction(mButtonBuildId);
    }
}

void StepGameBuildUnit::OnStart()
{
    TutorialInfoStep::OnStart();

    // move elements to front
    auto stage = sgl::sgui::Stage::Instance();
    stage->MoveChildToFront(mFocusArea);
    stage->MoveChildToFront(GetClickFilter());
    stage->MoveChildToFront(GetPanelInfo());
}

} // namespace game
