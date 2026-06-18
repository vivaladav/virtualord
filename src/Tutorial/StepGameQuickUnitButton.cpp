#include "Tutorial/StepGameQuickUnitButton.h"

#include "Widgets/Tutorial/FocusArea.h"
#include "Widgets/Tutorial/PanelClickFilter.h"
#include "Widgets/Tutorial/PanelInfoTutorial.h"
#include "Widgets/GameHUD.h"

#include <sgl/sgui/AbstractButton.h>
#include <sgl/sgui/ButtonsGroup.h>
#include <sgl/utilities/StringManager.h>

namespace game
{

StepGameQuickUnitButton::StepGameQuickUnitButton(GameHUD * hud, int indButton, const char * intro,
                                                 const sgl::core::Pointd2D & p0)
    : TutorialInfoStep(550, 200)
    , mFocusArea(new FocusArea)
{
    auto sm = sgl::utilities::StringManager::Instance();

    // FOCUS
    mFocusArea->SetCornersColorAction();
    mFocusArea->SetBlinking(true);
    mFocusArea->SetVisible(false);

    // INFO
    auto info = GetPanelInfo();

    info->SetPosition(p0.x, p0.y);

    if(intro != nullptr)
        info->AddInfoEntry(sm->GetCString(intro), 7.f, true, false);

    const auto group = hud->GetQuickUnitButtonsGroup();
    mButton = group->GetButton(indButton);

    info->AddActionEntry(sm->GetCString("TUT_GAME_QUICK_SEL_2"), 0.f, false, false, [this]
                        {
                            // CLICK FILTER
                            const int fX = mButton->GetScreenX();
                            const int fY = mButton->GetScreenY();
                            const int fW = mButton->GetWidth();
                            const int fH = mButton->GetHeight();

                            GetClickFilter()->SetScreenClickableArea(fX, fY, fW, fH);

                            // FOCUS
                            const int padding = 5;
                            const int f2X = fX - padding;
                            const int f2Y = fY - padding;
                            const int f2W = fW + (padding * 2);
                            const int f2H = fH + (padding * 2);

                            mFocusArea->SetScreenArea(f2X, f2Y, f2W, f2H);
                            mFocusArea->SetVisible(true);
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
