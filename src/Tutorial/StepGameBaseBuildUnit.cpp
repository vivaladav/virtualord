#include "Tutorial/StepGameBaseBuildUnit.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/DialogNewElement.h"
#include "Widgets/GameHUD.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/Stage.h>

namespace game
{

StepGameBaseBuildUnit::StepGameBaseBuildUnit(GameHUD * HUD)
    : mHUD(HUD)
{
    // CLICK FILTER
    mClickFilter = new PanelClickFilter;
    mClickFilter->SetEnabled(false);

    // FOCUS
    mFocusArea = new FocusArea;
    mFocusArea->SetCornersColor(colorTutorialFocusAction);
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    mInfo = new PanelInfoTutorial(480, 250);
    mInfo->SetEnabled(false);
    mInfo->SetVisible(false);
    mInfo->SetPosition(40, 810);

    mInfo->AddInfoEntry("You can choose what unit you want to build using this dialog.",
                        colorTutorialText, 6.f, true, false);
    mInfo->AddInfoEntry("You can only build worker units for now, so let's do that!",
                        colorTutorialText, 6.f, true, false);
    mInfo->AddInfoEntry("Click the button BUILD to create one.",
                        colorTutorialTextAction, 0.f, false, false, [this, HUD]
                        {
                            auto dialog = HUD->GetDialogNewElement();
                            auto btn = dialog->GetButtonBuild();

                            mButtonActId = btn->AddOnClickFunction([this]
                            {
                                SetDone();
                            });

                            // FOCUS
                            const int padding = 10;
                            const int fX = dialog->GetX() + btn->GetX() - padding;
                            const int fY = dialog->GetY() + btn->GetY() - padding;
                            const int fW = btn->GetWidth() + (padding * 2);
                            const int fH = btn->GetHeight() + (padding * 2);

                            mFocusArea->SetScreenArea(fX, fY, fW, fH);
                            mFocusArea->SetVisible(true);

                            // CLICK FILTER
                            mClickFilter->SetScreenClickableArea(fX, fY, fW, fH);
                        });
}

StepGameBaseBuildUnit::~StepGameBaseBuildUnit()
{
    delete mClickFilter;
    delete mFocusArea;
    delete mInfo;
}

void StepGameBaseBuildUnit::OnStart()
{
    // CLICK FILTER
    mClickFilter->SetEnabled(true);

    // INFO
    mInfo->SetEnabled(true);
    mInfo->SetVisible(true);
    mInfo->SetFocus();

    mInfo->StartInfo();

    // move elements to front
    auto stage = sgl::sgui::Stage::Instance();
    stage->MoveChildToFront(mFocusArea);
    stage->MoveChildToFront(mClickFilter);
    stage->MoveChildToFront(mInfo);
}

void StepGameBaseBuildUnit::OnEnd()
{
    auto dialog = mHUD->GetDialogNewElement();

    if(nullptr == dialog)
        return ;

    auto btn = dialog->GetButtonBuild();
    btn->RemoveClickFunction(mButtonActId);
}

} // namespace game
