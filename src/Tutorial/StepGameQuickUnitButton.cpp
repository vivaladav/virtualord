#include "Tutorial/StepGameQuickUnitButton.h"

#include "Tutorial/TutorialConstants.h"
#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"
#include "Widgets/GameHUD.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/ButtonsGroup.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameQuickUnitButton::StepGameQuickUnitButton(GameHUD * hud, int indButton)
    : TutorialInfoStep(550, 200)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColor(TutorialConstants::colorFocusAction);
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(450, 600);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_QUICK_SEL_1"),
                       TutorialConstants::colorText, 7.f, true, false);

    const auto group = hud->GetQuickUnitButtonsGroup();
    mButton = group->GetButton(indButton);

    info->AddInfoEntry(sm->GetCString("TUT_GAME_QUICK_SEL_2"),
                       TutorialConstants::colorTextAction, 0.f, false, false, [this]
                       {
                           // FOCUS
                           const int padding = 10;
                           const int fX = mButton->GetScreenX() - padding;
                           const int fY = mButton->GetScreenY() - padding;
                           const int fW = mButton->GetWidth() + (padding * 2);
                           const int fH = mButton->GetHeight() + (padding * 2);

                           mFocusArea->SetScreenArea(fX, fY, fW, fH);
                           mFocusArea->SetVisible(true);

                           // CLICK FILTER
                           GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);
                       });

    mClickId = mButton->AddOnToggleFunction([this](bool checked)
                                            {
                                                if(checked)
                                                    SetDone();
                                            });
}

StepGameQuickUnitButton::~StepGameQuickUnitButton()
{
    mButton->RemoveToggleFunction(mClickId);

    delete mFocusArea;
}

} // namespace game
