#include "Tutorial/StepGameBaseBuildUnit.h"

#include "Widgets/DialogNewElement.h"
#include "Widgets/GameHUD.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/Stage.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameBaseBuildUnit::StepGameBaseBuildUnit(GameHUD * HUD)
    : TutorialInfoStep(570, 200)
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

    info->SetPosition(600, 850);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_BASE_BUILD_UNIT_1"), 8.f, true, false);
    info->AddInfoEntry(sm->GetCString("TUT_GAME_BASE_BUILD_UNIT_2"), 7.f, true, false);
    info->AddActionEntry(sm->GetCString("TUT_GAME_BASE_BUILD_UNIT_3"), 0.f, false, false, [this, HUD]
                       {
                           auto dialog = HUD->GetDialogNewElement();
                           auto btn = dialog->GetButtonBuild();

                           mButtonActId = btn->AddOnClickFunction([this]
                                                                  {
                                                                      SetDone();
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
                           GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);
                       });
}

StepGameBaseBuildUnit::~StepGameBaseBuildUnit()
{
    delete mFocusArea;

    // remove click function
    auto dialog = mHUD->GetDialogNewElement();

    if(nullptr == dialog)
        return ;

    auto btn = dialog->GetButtonBuild();
    btn->RemoveClickFunction(mButtonActId);
}

void StepGameBaseBuildUnit::OnStart()
{
    TutorialInfoStep::OnStart();

    // move elements to front
    auto stage = sgl::sgui::Stage::Instance();
    stage->MoveChildToFront(mFocusArea);
    stage->MoveChildToFront(GetClickFilter());
    stage->MoveChildToFront(GetPanelInfo());
}

} // namespace game
